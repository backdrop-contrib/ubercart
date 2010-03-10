<?php
// $Id$

/**
 * @file
 * Roles administration menu items.
 */

/**
 * Create the header for the table/pager.
 */
function _uc_roles_expiration_header() {
  return array(
    array('data' => t('Username'), 'field' => 'u.name'),
    array('data' => t('Role'), 'field' => 'e.rid'),
    array('data' => t('Expiration date'), 'field' => 'e.expiration', 'sort' => 'asc'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
}

/**
 * Menu callback for viewing expirations.
 */
function uc_roles_expiration($form, &$form_state) {
  // Create the header for the pager.
  $header = _uc_roles_expiration_header();

  // Grab all the info to build the pager.
  $sql = 'SELECT * FROM {uc_roles_expirations} AS e INNER JOIN {users} AS u ON e.uid = u.uid';
  $sql .= tablesort_sql($header);
  $result = pager_query($sql, 50, 0, NULL);

  // Stick the expirations into the form.
  foreach ($result as $row) {
    $account = user_load($row->uid);
    $name = check_plain(format_username($account));
    $form['name'][$row->uid .' '. $row->rid] = array(
      '#theme' => 'username',
      '#account' => $account,
      '#name' => $name,
    );
    $form['role'][$row->uid .' '. $row->rid] = array('#markup' => _uc_roles_get_name($row->rid));
    $form['expiration'][$row->uid .' '. $row->rid] = array('#markup' => format_date($row->expiration, 'small'));
    $form['edit'][$row->uid .' '. $row->rid] = array(
      '#theme' => 'link',
      '#text' => t('edit'),
      '#href' => 'user/'. $row->uid .'/edit',
      '#options' => array(
        'fragment' => 'role-expiration-'. $row->rid,
        'query' => 'destination=admin%2Fuser%2Fuser%2Fexpiration',
      ),
    );
    $form['delete'][$row->uid .' '. $row->rid] = array(
      '#theme' => 'link',
      '#text' => t('delete'),
      '#href' => 'admin/user/user/expiration/delete/'. $row->uid .'/'. $row->rid,
    );
  }

  return $form;
}

/**
 * Theme user role expiration page.
 *
 * @ingroup themeable
 */
function theme_uc_roles_expiration($variables) {
  $form = $variables['form'];

  $header = _uc_roles_expiration_header();

  if (is_array($form['name'])) {
    foreach (element_children($form['name']) as $key) {
      $rows[] = array(
        drupal_render($form['name'][$key]),
        drupal_render($form['role'][$key]),
        drupal_render($form['expiration'][$key]),
        drupal_render($form['edit'][$key]),
        drupal_render($form['delete'][$key]),
      );
    }
  }
  else  {
    $rows[] = array(array('data' => t('No expirations set to occur'), 'colspan' => '5'));
  }

  // Render everything.
  $output .= theme('table', array('header' => $header, 'rows' => $rows));
  $output .= theme('pager');
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Form builder for role expirations.
 *
 * @ingroup forms
 * @see uc_roles_deletion_form_submit()
 */
function uc_roles_deletion_form($form, &$form_state, $account, $rid) {
  $expiration = db_query("SELECT expiration FROM {uc_roles_expirations} WHERE uid = :uid AND rid = :rid", array(':uid' => $account->uid, ':rid' => $rid))->fetchField();
  if ($expiration) {

    $role_name = _uc_roles_get_name($rid);

    $form['user'] = array('#type' => 'value', '#value' => format_username($account->name));
    $form['uid'] = array('#type' => 'value', '#value' => $account->uid);
    $form['role'] = array('#type' => 'value', '#value' => $role_name);
    $form['rid'] = array('#type' => 'value', '#value' => $rid);

    $form = confirm_form(
      $form,
      t('Delete expiration of %role_name role for the user !user?', array(
        '!user' => theme('username', array(
          'account' => $account,
          'name' => check_plain($account->name),
          'link_path' => 'user/' . $account->uid,
        )),
        '%role_name' => $role_name,
      )),
      'admin/user/user/expiration',
      t('Deleting the expiration will give !user privileges set by the %role_name role indefinitely unless manually removed.', array(
        '!user' => theme('username', array(
          'account' => $account,
          'name' => check_plain($account->name),
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
 * @see uc_roles_deletion_form()
 */
function uc_roles_deletion_form_submit($form, &$form_state) {
  uc_roles_delete(user_load($form_state['values']['uid']), $form_state['values']['rid']);

  drupal_goto('admin/user/user/expiration');
}

