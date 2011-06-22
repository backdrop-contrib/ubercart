<?php

/**
 * @file
 * Store administration menu items.
 */

/**
 * Menu callback which provides the store administration overview page.
 */
function uc_store_admin() {
  module_load_include('inc', 'system', 'system.admin');
  // Check for status report errors.
  if (system_status(TRUE) && user_access('administer site configuration')) {
    drupal_set_message(t('One or more problems were detected with your Drupal installation. Check the <a href="@status">status report</a> for more information.', array('@status' => url('admin/reports/status'))), 'error');
  }
  $blocks = array();
  if ($admin = db_query("SELECT menu_name, mlid FROM {menu_links} WHERE link_path = 'admin/store' AND module = 'system'")->fetchAssoc()) {
    $result = db_query("
      SELECT m.*, ml.*
      FROM {menu_links} ml
      INNER JOIN {menu_router} m ON ml.router_path = m.path
      WHERE ml.link_path != 'admin/help' AND menu_name = :menu_name AND ml.plid = :mlid AND hidden = 0", $admin, array('fetch' => PDO::FETCH_ASSOC));
    foreach ($result as $item) {
      _menu_link_translate($item);
      if (!$item['access']) {
        continue;
      }
      // The link 'description' either derived from the hook_menu 'description'
      // or entered by the user via menu module is saved as the title attribute.
      if (!empty($item['localized_options']['attributes']['title'])) {
        $item['description'] = $item['localized_options']['attributes']['title'];
      }
      $block = $item;
      $block['content'] = theme('admin_block_content', array('content' => system_admin_menu_block($item)));
      if (!empty($block['content'])) {
        $block['show'] = TRUE;
      }

      // Prepare for sorting as in function _menu_tree_check_access().
      // The weight is offset so it is always positive, with a uniform 5-digits.
      $blocks[(50000 + $item['weight']) . ' ' . $item['title'] . ' ' . $item['mlid']] = $block;
    }
  }

  ksort($blocks);
  $build['blocks'] = array(
    '#theme' => 'admin_page',
    '#blocks' => $blocks,
  );

  if ($results = module_invoke_all('uc_store_status')) {
    foreach ($results as $message) {
      switch ($message['status']) {
        case 'warning': $icon = 'alert.gif'; break;
        case 'error':   $icon = 'error.gif'; break;
        default:        $icon = 'info.gif';
      }

      $rows[] = array(
        'data' => array(
          array('data' => theme('image', array('path' => drupal_get_path('module', 'uc_store') . '/images/' . $icon))),
          array('data' => '<strong>' . $message['title'] . '</strong>', 'nowrap' => 'nowrap'),
          array('data' => $message['desc'], 'width' => '100%')
        ),
        'valign' => 'top',
      );
    }

    $build['status'] = array(
      '#theme' => 'table',
      '#caption' => '<h2>' . t('Store status') . '</h2>',
      '#rows' => $rows,
    );
  }

  return $build;
}

/**
 * Displays customer administration page.
 */
function uc_store_customers($message = NULL, $query = NULL, $page_length = 25) {
  if (!module_exists('uc_order')) {
    return t('You must enable the order module to track customers.');
  }

  $address = variable_get('uc_customer_list_address', 'billing');
  if ($address == 'shipping') {
    $address = 'delivery';
  }

  if (is_null($query)) {
    $query = uc_store_customer_query($page_length);
  }

  $header = array(
    t('View'),
    array('data' => t('Name'), 'field' => 'o.' . $address . '_last_name', 'sort' => 'asc'),
    array('data' => t('E-mail'), 'field' => 'u.mail'),
    array('data' => t('City'), 'field' => 'o.' . $address . '_city'),
    array('data' => t('ID'), 'field' => 'o.uid'),
  );
  $rows = array();

  $query = $query->extend('TableSort')
    ->orderByHeader($header);

  $result = $query->execute();

  foreach ($result as $customer) {
    $icons = l(uc_store_get_icon('admin/store/customers', TRUE),
               'user/' . $customer->uid,
               array('html' => TRUE, 'attributes' => array('title' => t('View user details.')))
             )
           . l(uc_store_get_icon('admin/store/orders', TRUE),
               'admin/store/customers/orders/' . $customer->uid,
               array('html' => TRUE, 'attributes' => array('title' => t("View customer's order.")))
             );
    if ($address == 'delivery') {
      $name = ucfirst($customer->delivery_last_name) . ', '
            . ucfirst($customer->delivery_first_name);
      $city = ucfirst($customer->delivery_city) . ', '
            . uc_get_zone_code($customer->delivery_zone);
    }
    else {
      $name = ucfirst($customer->billing_last_name) . ', '
            . ucfirst($customer->billing_first_name);
      $city = ucfirst($customer->billing_city) . ', '
            . uc_get_zone_code($customer->billing_zone);
    }
    if ($name == ', ') {
      $name = db_query("SELECT name FROM {users} WHERE uid = :uid", array(':uid' => $customer->uid))->fetchField();
      $name = t('User: !name', array('!name' => $name));
    }
    $rows[] = array(
      'data' => array(
        array('data' => $icons),
        array('data' => check_plain($name)),
        array('data' => check_plain($customer->mail)),
        array('data' => check_plain($city)),
        array('data' => $customer->uid)),
      'id' => 'customer-' . $customer->uid,
    );
  }

  drupal_add_js(drupal_get_path('module', 'uc_store') . '/uc_store.js');

  $build = array();

  $build['customers'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('width' => '100%', 'class' => array('uc-customer-table')),
  );
  $build['pager'] = array(
    '#theme' => 'pager',
    '#weight' => 5,
  );

  return $build;
}

/**
 * Assembles a query to list store customers.
 */
function uc_store_customer_query($page_length = 25) {
  $address = variable_get('uc_customer_list_address', 'billing');
  if ($address == 'shipping') {
    $address = 'delivery';
  }

  $query = db_select('uc_orders', 'o');
  $query->leftJoin('users', 'u', 'o.uid = u.uid');

  $o_uid = $query->addField('o', 'uid');
  $o_status = $query->addField('o', 'order_status');
  $query->fields('o', array(
      $address . '_first_name',
      $address . '_last_name',
      $address . '_city',
      $address . '_zone',
      $address . '_country'
    ))
    ->addField('u', 'mail');

  $query->condition('o.uid', 0, '>')
    ->condition('o.order_status', uc_order_status_list('general', TRUE), 'IN');

  $query = $query->extend('PagerDefault');
  $query->distinct()
    ->limit($page_length);

  return $query;
}

/**
 * Displays the customer search page.
 */
function uc_store_customer_search() {
  $build = array();
  $build['form'] = drupal_get_form('uc_store_customer_search_form');

  if (arg(4) == 'results') {
    $address = variable_get('uc_customer_list_address', 'billing');
    if ($address == 'shipping') {
      $address = 'delivery';
    }

    $query = uc_store_customer_query();

    $first_name = str_replace('*', '%', db_like(arg(5)));
    $last_name = str_replace('*', '%', db_like(arg(6)));
    $email = str_replace('*', '%', db_like(arg(7)));

    if ($first_name !== '0' && $first_name !== '%') {
      $query->condition('o.' . $address . '_first_name', $first_name, 'LIKE');
    }
    if ($last_name !== '0' && $last_name !== '%') {
      $query->condition('o.' . $address . '_last_name', $last_name, 'LIKE');
    }
    if ($email !== '0' && $email !== '%') {
      $query->addField('o', 'primary_email');
      $query->condition('o.primary_email', $email, 'LIKE');
    }

    $message = t('Search returned the following results:');

    $build['results'] = uc_store_customers($message, $query, 100);
  }

  return $build;
}

/**
 * Form builder for the customer search form.
 *
 * @see uc_store_customer_search_form_submit()
 * @ingroup forms
 */
function uc_store_customer_search_form($form, &$form_state) {
  $form['search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search options'),
    '#description' => t('Search for customers based on any of the following fields. Use * as a wildcard to match any character.') . '<br />' . t("For example, searching by last name for 's*' will return all customers whose last name starts with an s.") . '<br />' . t('(<em>Leave a field empty to ignore it in the search.</em>)'),
    '#collapsible' => TRUE,
    '#collapsed' => arg(4) == 'results' ? TRUE : FALSE,
    '#theme' => 'uc_store_customer_search_fieldset',
  );

  $form['search']['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
    '#default_value' => arg(5) != '0' ? arg(5) : '',
    '#size' => 24,
    '#maxlength' => 32,
  );

  $form['search']['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
    '#default_value' => arg(6) != '0' ? arg(6) : '',
    '#size' => 24,
    '#maxlength' => 32,
  );

  $form['search']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail'),
    '#default_value' => arg(7) != '0' ? arg(7) : '',
    '#size' => 24,
    '#maxlength' => 96,
  );

  $form['search']['actions'] = array('#type' => 'actions');
  $form['search']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );

  return $form;
}

/**
 * Returns HTML for the customer search fieldset
 *
 * @ingroup themeable
 */
function theme_uc_store_customer_search_fieldset($variables) {
  $form = $variables['form'];

  $output = '<table><tbody style="border: 0px;"><tr><td>';
  $output .= drupal_render($form['first_name']);

  $output .= '</td><td>';
  $output .= drupal_render($form['last_name']);

  $output .= '</td><td>';
  $output .= drupal_render($form['email']);

  $output .= '</td><td>';
  $output .= drupal_render($form['submit']);

  $output .= '</td></tr></tbody></table>';
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Form submission handler for uc_store_customer_search_form().
 *
 * @see uc_store_customer_search_form()
 */
function uc_store_customer_search_form_submit($form, &$form_state) {
  if (strlen(trim($form_state['values']['first_name'])) == 0) {
    $first_name = '0';
  }
  else {
    $first_name = strtolower(trim($form_state['values']['first_name']));
  }

  if (strlen(trim($form_state['values']['last_name'])) == 0) {
    $last_name = '0';
  }
  else {
    $last_name = strtolower(trim($form_state['values']['last_name']));
  }

  if (strlen(trim($form_state['values']['email'])) == 0) {
    $email = '0';
  }
  else {
    $email = strtolower(trim($form_state['values']['email']));
  }

  drupal_goto('admin/store/customers/search/results/' . $first_name . '/'
            . $last_name . '/' . $email);
}

/**
 * Displays main reports page.
 */
function uc_store_reports() {
  $menu = menu_get_item('admin/store/reports');
  $content = system_admin_menu_block($menu);

  $build = array();
  $build['message'] = array(
    '#markup' => (module_exists('uc_reports')) ? t('Various reports generated by Ubercart modules can be found here. Click the links below to view the reports.') : t('Various reports generated by Ubercart modules can be found here. Click the links below to view the reports. To view core Ubercart statistics enable the <strong>Reports</strong> module on the <a href="!url">module administration page</a>', array('!url' => url('admin/modules'))),
  );
  $build['menu'] = array(
    '#theme' => 'admin_block_content',
    '#content' => $content,
    '#weight' => 5,
  );

  return $build;
}

/**
 * Displays store configuration page.
 */
function uc_store_configuration_page() {
  $menu = menu_get_item('admin/store/settings');
  $content = system_admin_menu_block($menu);

  $build['menu'] = array(
    '#theme' => 'admin_block_content',
    '#content' => $content,
  );

  return $build;
}

/**
 * Displays store help page.
 */
function uc_store_ubercart_help() {
  $build = array();
  $build['description'] = array(
    '#markup' => '<p>' . t('Use the following links to find documentation and support:') . '</p>',
  );

  $items = array();
  $items[] = l(t("Ubercart User's Guide"), 'http://www.ubercart.org/docs/user');
  $items[] = l(t('Support Forums'), 'http://www.ubercart.org/forum');
  $items[] = l(t('Drupal Handbook'), 'http://drupal.org/node/258');
  $build['links'] = array(
    '#theme' => 'item_list',
    '#items' => $items,
  );

  return $build;
}

/**
 * Displays the tokens help page.
 */
function uc_store_ubercart_help_tokens() {
  $build = array();
  $build['description'] = array(
    '#markup' => '<p>' . t('Tokens are bracketed phrases you can use in certain text fields and boxes as placeholders for some other text. Tokens represent things like store variables, links to certain pages, order information, etc.  Tokens are used by including the token listed below in a text field that uses them.  The description for the textfields will alert you to which groups of tokens listed below may be used.') . '</p>',
  );

  return $build;
}

/**
 * Imports settings from a country file.
 *
 * @see uc_country_import_form_submit()
 * @ingroup forms
 */
function uc_country_import_form($form, &$form_state) {
  $countries = db_query("SELECT * FROM {uc_countries} ORDER BY country_name ASC")->fetchAll();
  $files = _uc_country_import_list();

  $header = array(t('Country'), t('Code'), t('Version'), t('Operations'));
  $rows = array();
  if (is_array($countries)) {
    foreach ($countries as $country) {
      $row = array(
        $country->country_name,
        $country->country_iso_code_3,
        array('data' => abs($country->version), 'align' => 'center')
      );

      $ops = array();
      if ($country->version < 0) {
        $ops[] = l(t('enable'), 'admin/store/settings/countries/' . $country->country_id . '/enable');
      }
      else {
        $ops[] = l(t('disable'), 'admin/store/settings/countries/' . $country->country_id . '/disable');
      }
      if ($country->version < $files[$country->country_id]['version'] && $country->version > 0) {
        $ops[] = l(t('update'), 'admin/store/settings/countries/' . $country->country_id . '/update/' . $files[$country->country_id]['version']);
      }
      $ops[] = l(t('remove'), 'admin/store/settings/countries/' . $country->country_id . '/remove');
      $row[] = implode(' ', $ops);

      $rows[] = $row;
      unset($files[$country->country_id]);
    }
  }

  $import_list = array();
  foreach ($files as $file) {
    $import_list[$file['file']] = $file['file'];
  }
  if (is_array($import_list)) {
    ksort($import_list);
  }

  if (is_array($import_list)) {
    $options = $import_list;
  }
  else {
    $options = array(t('-None available-'));
  }

  $form['country_import'] = array(
    '#title' => t('Import countries'),
    '#type' => 'fieldset',
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
  );

  $form['country_import']['text'] = array(
    '#markup' => '<p>' . t('To import new country data, select it in the list and click the import button. If you are using a custom or contributed import file, it must be placed in the Ubercart folder uc_store/countries.') . '</p>',
  );
  $form['country_import']['import_file'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#options' => $options,
    '#disabled' => !is_array($import_list),
    '#multiple' => is_array($import_list),
    '#size' => min(10, count($options)),
  );
  $form['country_import']['actions'] = array('#type' => 'actions');
  $form['country_import']['actions']['import_button'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
    '#disabled' => !is_array($import_list),
  );
  $form['country_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );

  return $form;
}

/**
 * Form submission handler for uc_country_import_form().
 *
 * @see uc_country_import_form()
 */
function uc_country_import_form_submit($form, &$form_state) {
  $files = $form_state['values']['import_file'];

  foreach ($files as $file) {
    if (uc_country_import($file)) {
      drupal_set_message(t('Country file @file imported.', array('@file' => $file)));
    }
    else {
      drupal_set_message(t('Country file @file could not import or had no install function.', array('@file' => $file)), 'error');
    }
  }
}

/**
 * Form builder to set country address formats.
 *
 * @see uc_country_formats_form_submit()
 * @ingroup forms
 */
function uc_country_formats_form($form, &$form_state) {
  $form['instructions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Address format variables'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $header = array(t('Variable'), t('Description'));
  $rows = array(
    array('!first_name', t("Customer's first name")),
    array('!last_name', t("Customer's last name")),
    array('!company', t('Company name')),
    array('!street1', t('First street address field')),
    array('!street2', t('Second street address field')),
    array('!city', t('City name')),
    array('!zone_name', t('Full name of the zone')),
    array('!zone_code', t('Abbreviation of the zone')),
    array('!postal_code', t('Postal code')),
    array('!country_name', t('Name of the country')),
    array('!country_code2', t('2 digit country abbreviation')),
    array('!country_code3', t('3 digit country abbreviation')),
  );
  $form['instructions']['text'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#prefix' => '<div><p>' . t('The following variables should be used in configuring addresses for the countries you ship to:') . '</p>',
    '#suffix' => '<p>' . t('Adding _if to any country variable will make it display only for addresses whose country is different than the default store country.') . '</p></div>',
  );

  $countries = db_query("SELECT * FROM {uc_countries} ORDER BY country_name ASC")->fetchAll();

  if (is_array($countries)) {
    $form['countries'] = array(
      '#type' => 'vertical_tabs',
      '#tree' => TRUE,
    );
    foreach ($countries as $country) {
      $form['countries'][$country->country_id] = array(
        '#type' => 'fieldset',
        '#title' => $country->country_name,
        '#group' => 'country',
      );
      $form['countries'][$country->country_id]['address_format'] = array(
        '#type' => 'textarea',
        '#title' => t('@country address format', array('@country' => $country->country_name)),
        '#default_value' => variable_get('uc_address_format_' . $country->country_id, ''),
        '#description' => t('Use the variables mentioned in the instructions to format an address for this country.'),
        '#rows' => 6,
      );
    }
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit changes'),
  );

  return $form;
}

/**
 * Form submission handler for uc_country_formats_form().
 *
 * @see uc_country_formats_form()
 */
function uc_country_formats_form_submit($form, &$form_state) {
  foreach ($form_state['values']['countries'] as $country_id => $data) {
    variable_set('uc_address_format_' . $country_id, $data['address_format']);
  }
  drupal_set_message(t('Country settings saved.'));
}

/**
 * Form builder for store settings.
 *
 * @see uc_store_settings_form_validate()
 * @ingroup forms
 */
function uc_store_settings_form($form, &$form_state) {
  $form['store']['#type'] = 'vertical_tabs';

  $form['basic'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic information'),
    '#group' => 'store',
  );
  $form['basic']['uc_store_name'] = uc_textfield(t('Store name'), variable_get('uc_store_name', NULL), FALSE, NULL, 64);
  $form['basic']['uc_store_owner'] = uc_textfield(t('Store owner'), variable_get('uc_store_owner', NULL), FALSE, NULL, 64);
  $form['basic']['uc_store_email'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail address'),
    '#description' => NULL,
    '#size' => 32,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#default_value' => variable_get('uc_store_email', NULL),
  );
  $form['basic']['uc_store_email_include_name'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include the store name in the "From" line of store e-mails.'),
    '#description' => t('May not be available on all server configurations. Turn off if this causes problems.'),
    '#default_value' => variable_get('uc_store_email_include_name', TRUE),
  );
  $form['basic']['uc_store_phone'] = uc_textfield(t('Phone number'), variable_get('uc_store_phone', NULL), FALSE);
  $form['basic']['uc_store_fax'] = uc_textfield(t('Fax number'), variable_get('uc_store_fax', NULL), FALSE);
  $form['basic']['uc_store_help_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Store help page'),
    '#description' => t('The Drupal page for the store help link.'),
    '#default_value' => variable_get('uc_store_help_page', ''),
    '#size' => 32,
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
  );

  $form['address'] = array(
    '#type' => 'fieldset',
    '#title' => t('Store address'),
    '#group' => 'store',
  );
  $form['address']['address'] = array(
    '#type' => 'uc_address',
    '#default_value' => array(
      'uc_store_street1' => variable_get('uc_store_street1', ''),
      'uc_store_street2' => variable_get('uc_store_street2', ''),
      'uc_store_city' => variable_get('uc_store_city', ''),
      'uc_store_zone' => variable_get('uc_store_zone', 0),
      'uc_store_country' => isset($form_state['values']) ? $form_state['values']['uc_store_country'] : uc_store_default_country(),
      'uc_store_postal_code' => variable_get('uc_store_postal_code', ''),
    ),
    '#required' => FALSE,
    '#key_prefix' => 'uc_store',
  );

  $form['currency'] = array(
    '#type' => 'fieldset',
    '#title' => t('Currency format'),
    '#group' => 'store',
  );
  $form['currency']['uc_currency_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Default currency'),
    '#description' => t('While not used directly in formatting, the currency code is used by other modules as the primary currency for your site.  Enter here your three character <a href="!url">ISO 4217</a> currency code.', array('!url' => 'http://en.wikipedia.org/wiki/ISO_4217#Active_codes')),
    '#default_value' => variable_get('uc_currency_code', 'USD'),
    '#maxlength' => 3,
    '#size' => 5,
  );
  $form['currency']['example'] = array(
    '#type' => 'textfield',
    '#title' => t('Current format'),
    '#value' => uc_currency_format(1000.1234),
    '#disabled' => TRUE,
    '#size' => 10,
  );
  $form['currency']['uc_currency_sign'] = uc_textfield(t('Currency Sign'), variable_get('uc_currency_sign', '$'), FALSE, NULL, 10, 10);
  $form['currency']['uc_sign_after_amount'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display currency sign after amount.'),
    '#default_value' => variable_get('uc_sign_after_amount', FALSE),
  );
  $form['currency']['uc_currency_thou'] = uc_textfield(t('Thousands Marker'), variable_get('uc_currency_thou', ','), FALSE, NULL, 10, 10);
  $form['currency']['uc_currency_dec'] = uc_textfield(t('Decimal Marker'), variable_get('uc_currency_dec', '.'), FALSE, NULL, 10, 10);
  $form['currency']['uc_currency_prec'] = array(
    '#type' => 'select',
    '#title' => t('Number of decimal places'),
    '#options' => drupal_map_assoc(array(0, 1, 2)),
    '#default_value' => variable_get('uc_currency_prec', 2),
  );

  $form['weight'] = array(
    '#type' => 'fieldset',
    '#title' => t('Weight format'),
    '#description' => t('Supply a format string for each unit. !value represents the weight value.'),
    '#group' => 'store',
  );
  $units = array(
    'lb' => t('Pounds'),
    'oz' => t('Ounces'),
    'kg' => t('Kilograms'),
    'g' => t('Grams'),
  );
  $form['weight']['uc_weight_unit'] = array(
    '#type' => 'select',
    '#title' => t('Default unit of measurement'),
    '#default_value' => variable_get('uc_weight_unit', 'lb'),
    '#options' => $units,
  );
  foreach ($units as $unit => $name) {
    $form['weight']['uc_weight_format_' . $unit] = array(
      '#type' => 'textfield',
      '#title' => t('@unit format string', array('@unit' => $name)),
      '#default_value' => variable_get('uc_weight_format_' . $unit, '!value ' . $unit),
    );
  }

  $form['length'] = array(
    '#type' => 'fieldset',
    '#title' => t('Length format'),
    '#description' => t('Supply a format string for each unit. !value represents the length value.'),
    '#group' => 'store',
  );
  $units = array(
    'in' => t('Inches'),
    'ft' => t('Feet'),
    'cm' => t('Centimeters'),
    'mm' => t('Millimeters'),
  );
  $form['length']['uc_length_unit'] = array(
    '#type' => 'select',
    '#title' => t('Default unit of measurement'),
    '#default_value' => variable_get('uc_length_unit', 'in'),
    '#options' => $units,
  );
  foreach ($units as $unit => $name) {
    $form['length']['uc_length_format_' . $unit] = array(
      '#type' => 'textfield',
      '#title' => t('@unit format string', array('@unit' => $name)),
      '#default_value' => variable_get('uc_store_length_format_' . $unit, '!value ' . $unit),
    );
  }

  $form['display'] = array(
    '#type' => 'fieldset',
    '#title' => t('Display settings'),
    '#group' => 'store',
  );
  $form['display']['uc_customer_list_address'] = array(
    '#type' => 'radios',
    '#title' => t('Primary customer address'),
    '#description' => t('Select the address to be used on customer lists and summaries.'),
    '#options' => array(
      'billing' => t('Billing address'),
      'shipping' => t('Shipping address'),
    ),
    '#default_value' => variable_get('uc_customer_list_address', 'billing'),
  );
  $form['display']['uc_footer_message'] = array(
    '#type' => 'radios',
    '#title' => t('Footer message for store pages'),
    '#options' => array_merge(
      array(0 => t('Randomly select a message from the list below.')),
      _uc_store_footer_options(),
      array('none' => t('(Do not display a message in the footer.)'))
    ),
    '#default_value' => variable_get('uc_footer_message', 0),
  );

  return system_settings_form($form);
}

/**
 * Validate store e-mail address for uc_store_settings_form().
 *
 * @see uc_store_settings_form()
 */
function uc_store_settings_form_validate($form, &$form_state) {
  $mail = trim($form_state['values']['uc_store_email']);
  if (!valid_email_address($mail)) {
    form_set_error('uc_store_email', t('The e-mail address %mail is not valid.', array('%mail' => $mail)));
  }
}

/**
 * Form to enter initials for an administrative user.
 *
 * @see uc_store_initials_submit()
 * @ingroup forms
 */
function uc_store_initials($form, &$form_state) {
  $form['username'] = array(
    '#type' => 'textfield',
    '#title' => t('User name'),
    '#description' => t('Enter the name of the user whose initials you want to adjust.'),
    '#required' => TRUE,
    '#size' => 32,
    '#autocomplete_path' => 'user/autocomplete',
  );
  $form['initials'] = array(
    '#type' => 'textfield',
    '#title' => t('Initials'),
    '#description' => t('Enter initials or leave blank to erase current initials.'),
    '#size' => 6,
    '#maxlength' => 32,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Form submission handler for uc_store_initials().
 *
 * @see uc_store_initials()
 */
function uc_store_initials_submit($form, &$form_state) {
  $result = db_query("SELECT uid FROM {users} WHERE name = :name", array(':name' => $form_state['values']['username']));
  if ($user = $result->fetchObject()) {
    if ($form_state['values']['initials'] == '') {
      variable_del('user_initials_' . $user->uid);
      drupal_set_message(t('Initials for !username deleted.', array('!username' => $form_state['values']['username'])));
    }
    else {
      variable_set('user_initials_' . $user->uid, $form_state['values']['initials']);
      drupal_set_message(t('Initials for !username set to !initials.',
                           array('!username' => $form_state['values']['username'],
                                 '!initials' => $form_state['values']['initials'])));
    }
  }
}

/**
 * Displays a list of orders made by a customer.
 *
 * $param $uid
 *   The user id of the customer.
 */
function uc_store_customer_orders($uid) {
  $result = db_select('uc_orders')->extend('PagerDefault')
    ->fields('uc_orders')
    ->condition('uid', $uid)
    ->condition('order_status', uc_order_status_list('general', TRUE), 'IN')
    ->orderBy('created', 'DESC')
    ->limit(50)
    ->execute();

  $header = array(t('View'), t('Order ID'), t('Date'), t('Billing name'),
                  t('Shipping name'), t('Items'), t('Total'));

  $totals = array('orders' => 0, 'items' => 0, 'total' => 0);
  foreach ($result as $order) {
    $icons = l(uc_store_get_icon('file:order_view'),
               'admin/store/orders/' . $order->order_id,
               array('html' => TRUE, 'attributes' => array('title' => t("View order !order_id.", array('!order_id' => $order->order_id))))
             )
           . l(uc_store_get_icon('file:order_edit'),
               'admin/store/orders/' . $order->order_id . '/edit',
               array('html' => TRUE, 'attributes' =>
                 array(
                   'title' => t("Edit order !order_id.",
                     array('!order_id' => $order->order_id)
                   )
                 )
               )
             );
    $bname = ucfirst($order->billing_first_name) . ' '
           . ucfirst($order->billing_last_name);
    $sname = ucfirst($order->delivery_first_name) . ' '
           . ucfirst($order->delivery_last_name);
    $item_count = db_query("SELECT COUNT(*) FROM {uc_order_products} WHERE order_id = :order_id", array(':order_id' => $order->order_id))->fetchField();

    $totals['orders'] += 1;
    $totals['items'] += $item_count;
    $totals['total'] += $order->order_total;

    $rows[] = array(
      'data' => array(
        array('data' => $icons),
        array('data' => $order->order_id),
        array('data' => format_date($order->created, 'uc_store')),
        array('data' => check_plain($bname)),
        array('data' => check_plain($sname)),
        array('data' => $item_count['COUNT(*)']),
        array('data' => array('#theme' => 'uc_price', '#price' => $order->order_total)),
      ),
      'id' => 'order-' . $order->order_id,
    );
  }
  if (empty($rows)) {
    $rows[] = array(
      array('data' => t('No orders found.'), 'colspan' => 7)
    );
  }

  drupal_add_js(drupal_get_path('module', 'uc_store') . '/uc_store.js');

  $build = array();

  if (user_access('create orders')) {
    $build['order_link'] = array(
      '#markup' => '<p>' . l(t('Create an order for this customer.'), 'admin/store/orders/create/' . $uid) . '</p>',
      '#weight' => -10,
    );
  }

  $build['description'] = array(
    '#markup' => '<p>' . t('!totals_orders orders shown match that account with !totals_items items purchased and !totals_total spent:',
       array(
         '!totals_orders' => $totals['orders'],
         '!totals_items' => $totals['items'],
         '!totals_total' => uc_currency_format($totals['total']),
       )) . '</p>',
  );
  $build['orders'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('width' => '100%', 'class' => array('uc-cust-orders-table')),
  );
  $build['pager'] = array(
    '#theme' => 'pager',
    '#weight' => 5,
  );

  return $build;
}

/**
 * Disables a country so it remains installed but is no longer selectable.
 *
 * @param $country_id
 *   The ISO 3166-1 numeric country code.
 */
function uc_country_disable($country_id) {
  $result = db_query("SELECT * FROM {uc_countries} WHERE country_id = :id", array(':id' => $country_id));
  if ($country = $result->fetchObject()) {
    if ($country->version > 0) {
      db_update('uc_countries')
        ->fields(array(
          'version' => -$country->version,
        ))
        ->condition('country_id', $country_id)
        ->execute();
      drupal_set_message(t('!country disabled.', array('!country' => $country->country_name)));
    }
    else {
      drupal_set_message(t('!country is already disabled.', array('!country' => $country->country_name)), 'error');
    }
  }
  else {
    drupal_set_message(t('Attempted to disable an invalid country.'), 'error');
  }
  drupal_goto('admin/store/settings/countries');
}

/**
 * Enables a disabled country.
 *
 * @param $country_id
 *   The ISO 3166-1 numeric country code.
 */
function uc_country_enable($country_id) {
  $result = db_query("SELECT * FROM {uc_countries} WHERE country_id = :id", array(':id' => $country_id));
  if ($country = $result->fetchObject()) {
    if ($country->version < 0) {
      db_update('uc_countries')
        ->fields(array(
          'version' => abs($country->version),
        ))
        ->condition('country_id', $country_id)
        ->execute();
      drupal_set_message(t('@country enabled.', array('@country' => $country->country_name)));
    }
    else {
      drupal_set_message(t('@country is already enabled.', array('@country' => $country->country_name)), 'error');
    }
  }
  else {
    drupal_set_message(t('Attempted to enable an invalid country.'), 'error');
  }
  drupal_goto('admin/store/settings/countries');
}

/**
 * Form to completely remove a country.
 *
 * @param $country_id
 *   The ISO 3166-1 numeric country code.
 *
 * @see uc_country_remove_form_submit()
 * @ingroup forms
 */
function uc_country_remove_form($form, &$form_state, $country_id) {
  // Fetch the country name from the database.
  $country = db_query("SELECT country_name FROM {uc_countries} WHERE country_id = :id", array(':id' => $country_id))->fetchField();

  // If orders exist for this country, show a warning message prior to removal.
  if (isset($form_state['triggering_element']) && $form_state['triggering_element']['#value'] != t('Remove') && module_exists('uc_order')) {
    $count = db_query("SELECT COUNT(order_id) FROM {uc_orders} WHERE delivery_country = :delivery_country OR billing_country = :billing_country", array(':delivery_country' => $country_id, ':billing_country' => $country_id))->fetchField();
    if ($count > 0) {
      drupal_set_message(t('Warning: @count orders were found with addresses in this country. Removing this country now will cause errors to show on those order pages. You might consider simply disabling this country instead.', array('@count' => $count)), 'error');
    }
  }

  // Store the country ID in the form array for processing.
  $form['country_id'] = array(
    '#type' => 'value',
    '#value' => $country_id,
  );

  return confirm_form($form, t('Are you sure you want to remove @country from the system?', array('@country' => $country)), 'admin/store/settings/countries', NULL, t('Remove'));
}

/**
 * Form submission handler for uc_country_remove_form().
 *
 * @see uc_country_remove_form()
 */
function uc_country_remove_form_submit($form, &$form_state) {
  $country_id = $form_state['values']['country_id'];

  $result = db_query("SELECT * FROM {uc_countries} WHERE country_id = :id", array(':id' => $country_id));
  if (!($country = $result->fetchObject())) {
    drupal_set_message(t('Attempted to remove an invalid country.'), 'error');
    drupal_goto('admin/store/settings/countries');
  }

  db_delete('uc_countries')
    ->condition('country_id', $country_id)
    ->execute();
  db_delete('uc_zones')
    ->condition('zone_country_id', $country_id)
    ->execute();
  variable_del('uc_address_format_' . $country_id);

  $func_base = _uc_country_import_include($country_id, $country->version);
  if ($func_base !== FALSE) {
    $func = $func_base . '_uninstall';
    if (function_exists($func)) {
      $func();
    }
  }

  drupal_set_message(t('!country removed.', array('!country' => $country->country_name)));
  drupal_goto('admin/store/settings/countries');
}

/**
 * Updates a country definition to a specific CIF file version.
 *
 * @param $country_id
 *   The ISO 3166-1 numeric country code.
 * @param $version
 *   Version number of CIF file.
 */
function uc_country_update($country_id, $version) {
  $result = db_query("SELECT * FROM {uc_countries} WHERE country_id = :id", array(':id' => $country_id));
  if (!($country = $result->fetchObject())) {
    drupal_set_message(t('Attempted to update an invalid country.'));
    drupal_goto('admin/store/settings/countries');
  }

  if ($version < $country->version) {
    drupal_set_message(t('You cannot update to a previous version.'));
    drupal_goto('admin/store/settings/countries');
  }

  $func_base = _uc_country_import_include($country_id, $version);
  if ($func_base !== FALSE) {
    $func = $func_base . '_update';
    if (function_exists($func)) {
      for ($i = $country->version; $i <= $version; $i++) {
        $func($i);
      }
    }

    db_update('uc_countries')
      ->fields(array(
        'version' => $version,
      ))
      ->condition('country_id', $country_id)
      ->execute();
    drupal_set_message(t('Country update complete.'));
  }
  else {
    drupal_set_message(t('Attempted to update an invalid country.'));
  }

  drupal_goto('admin/store/settings/countries');
}
