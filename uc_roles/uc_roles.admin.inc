<?php
/**
 * @file
 * Roles administration menu items.
 */

/**
 * Creates the header for the table/pager.
 */
function _uc_roles_expiration_header() {
  return array(
    array('data' => t('Username'), 'field' => 'u.name'),
    array(
      'data' => t('Role'),
      'field' => 'e.role',
    ),
    array(
      'data' => t('Expiration date'),
      'field' => 'e.expiration',
      'sort' => 'asc',
    ),
    array(
      'data' => t('Operations'),
    ),
  );
}

/**
 * Menu callback for viewing expirations.
 */
function uc_roles_expiration($form, &$form_state) {
  // Create the header for the pager.
  $header = _uc_roles_expiration_header();

  // Grab all the info to build the pager.
  $query = db_select('uc_roles_expirations', 'e')->extend('PagerDefault')->extend('TableSort')
    ->fields('e')
    ->limit(50)
    ->orderByHeader($header);

  $query->join('users', 'u', 'e.uid = u.uid');
  $query->fields('u');

  $result = $query->execute();

  // Stick the expirations into the form.
  foreach ($result as $row) {
    $account = user_load($row->uid);
    $name = check_plain(user_format_name($account));
    $form['name'][$row->uid . ' ' . $row->role] = array(
      '#theme' => 'username',
      '#account' => $account,
      '#name' => $name,
    );
    $form['role'][$row->uid . ' ' . $row->role] = array('#markup' => check_plain(_uc_roles_get_name($row->role)));
    $form['expiration'][$row->uid . ' ' . $row->role] = array('#markup' => format_date($row->expiration, 'short'));
    $form['operations'][$row->uid . ' ' . $row->role] = array(
      '#type' => 'operations',
      '#links' => array(
        array(
          'title' => t('edit'),
          'href' => 'user/' . $row->uid . '/edit',
          'fragment' => 'role-expiration-' . $row->role,
          'query' => array('destination' => 'admin/people/expiration'),
        ),
        array(
          'title' => t('delete'),
          'href' => 'admin/people/expiration/delete/' . $row->uid . '/' . $row->role,
        ),
      ),
    );
  }

  return $form;
}

/**
 * Themes user role expiration page.
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @ingroup themeable
 */
function theme_uc_roles_expiration($variables) {
  $form = $variables['form'];

  $header = _uc_roles_expiration_header();
  $rows = array();
  if (isset($form['name'])) {
    foreach (element_children($form['name']) as $key) {
      $rows[] = array(
        backdrop_render($form['name'][$key]),
        backdrop_render($form['role'][$key]),
        backdrop_render($form['expiration'][$key]),
        backdrop_render($form['operations'][$key]),
      );
    }
  }

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => t('No expirations set to occur'),
  ));
  $output .= theme('pager');
  $output .= backdrop_render_children($form);

  return $output;
}

/**
 * Form builder for role deletions.
 *
 * @see uc_roles_deletion_form_submit()
 * @ingroup forms
 */
function uc_roles_deletion_form($form, &$form_state, $account, $role) {
  $expiration = db_query("SELECT expiration FROM {uc_roles_expirations} WHERE uid = :uid AND role = :role", array(':uid' => $account->uid, ':role' => $role))->fetchField();
  if ($expiration) {

    $role_name = _uc_roles_get_name($role);

    $form['user'] = array(
      '#type' => 'value',
      '#value' => user_format_name($account),
    );
    $form['uid'] = array(
      '#type' => 'value',
      '#value' => $account->uid,
    );
    $form['role'] = array(
      '#type' => 'value',
      '#value' => $role_name,
    );
    $form['role'] = array(
      '#type' => 'value',
      '#value' => $role,
    );

    $form = confirm_form(
      $form,
      t('Delete expiration of %role_name role for the user !user?', array(
        '!user' => theme('username', array(
          'account' => $account,
          'name' => check_plain(user_format_name($account)),
          'link_path' => 'user/' . $account->uid,
        )),
        '%role_name' => $role_name,
      )),
      'admin/people/expiration',
      t('Deleting the expiration will give !user privileges set by the %role_name role indefinitely unless manually removed.', array(
        '!user' => theme('username', array(
          'account' => $account,
          'name' => check_plain(user_format_name($account)),
          'link_path' => 'user/' . $account->uid,
        )),
        '%role_name' => $role_name,
      )),
      t('Yes'),
      t('No')
    );
  }
  else {
    $form['error'] = array(
      '#markup' => t('Invalid user id or role id.'),
    );
  }

  return $form;
}

/**
 * Submission handler for uc_roles_deletion_form().
 *
 * @see uc_roles_deletion_form()
 */
function uc_roles_deletion_form_submit($form, &$form_state) {
  uc_roles_delete(user_load($form_state['values']['uid']), $form_state['values']['role']);

  backdrop_goto('admin/people/expiration');
}
