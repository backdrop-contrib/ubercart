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
 * Displays main reports page.
 */
function uc_store_reports() {
  $menu = menu_get_item('admin/store/reports');
  $content = system_admin_menu_block($menu);

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
 * Imports settings from a country file.
 *
 * @see uc_country_import_form_submit()
 * @ingroup forms
 */
function uc_country_import_form($form, &$form_state) {
  $countries = array();
  $result = db_query("SELECT * FROM {uc_countries}");
  foreach ($result as $country) {
    $countries[t($country->country_name)] = $country;
  }
  uksort($countries, 'strnatcasecmp');
  $files = _uc_country_import_list();

  $header = array(t('Country'), t('Code'), t('Version'), t('Operations'));
  $rows = array();
  if (is_array($countries)) {
    foreach ($countries as $country) {
      $row = array(
        t($country->country_name),
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

  if (!empty($import_list)) {
    ksort($import_list);

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
      '#options' => $import_list,
      '#multiple' => TRUE,
      '#size' => min(10, count($import_list)),
    );
    $form['country_import']['actions'] = array('#type' => 'actions');
    $form['country_import']['actions']['import_button'] = array(
      '#type' => 'submit',
      '#value' => t('Import'),
    );
  }

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

  $countries = array();
  $result = db_query("SELECT * FROM {uc_countries}");
  foreach ($result as $country) {
    $countries[t($country->country_name)] = $country;
  }
  uksort($countries, 'strnatcasecmp');

  if (is_array($countries)) {
    $form['countries'] = array(
      '#type' => 'vertical_tabs',
      '#tree' => TRUE,
    );
    foreach ($countries as $country) {
      $form['countries'][$country->country_id] = array(
        '#type' => 'fieldset',
        '#title' => t($country->country_name),
        '#group' => 'country',
      );
      $form['countries'][$country->country_id]['address_format'] = array(
        '#type' => 'textarea',
        '#title' => t('@country address format', array('@country' => t($country->country_name))),
        '#default_value' => variable_get('uc_address_format_' . $country->country_id, ''),
        '#description' => t('Use the variables mentioned in the instructions to format an address for this country.'),
        '#rows' => 7,
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
 * Form to configure address fields.
 *
 * @ingroup forms
 * @see uc_store_address_fields_form_submit()
 * @see uc_store_address_fields_form_reset()
 * @see theme_uc_store_address_fields_form()
 */
function uc_store_address_fields_form($form, &$form_state) {
  $form['fields'] = array(
    '#tree' => TRUE,
  );

  $fields = array(
    'first_name' => t('First name'),
    'last_name' => t('Last name'),
    'company' => t('Company'),
    'street1' => t('Street address 1'),
    'street2' => t('Street address 2'),
    'city' => t('City'),
    'zone' => t('State/Province'),
    'country' => t('Country'),
    'postal_code' => t('Postal code'),
    'phone' => t('Phone number'),
  );
  $current = variable_get('uc_address_fields', drupal_map_assoc(array('first_name', 'last_name', 'phone', 'company', 'street1', 'street2', 'city', 'zone', 'postal_code', 'country')));
  $required = variable_get('uc_address_fields_required', drupal_map_assoc(array('first_name', 'last_name', 'street1', 'city', 'zone', 'postal_code', 'country')));
  $weight = uc_store_address_field_weights();
  foreach ($fields as $field => $label) {
    $form['fields'][$field]['enabled'] = array(
      '#type' => 'checkbox',
      '#default_value' => isset($current[$field]) ? TRUE : FALSE,
    );
    $form['fields'][$field]['required'] = array(
      '#type' => 'checkbox',
      '#default_value' => isset($required[$field]) ? TRUE : FALSE,
    );
    $form['fields'][$field]['weight'] = array(
      '#type' => 'weight',
      '#default_value' => (isset($weight[$field])) ? $weight[$field] : 0,
      '#attributes' => array('class' => array('uc-store-address-fields-weight')),
    );
    $form['fields'][$field]['default'] = array(
      '#markup' => $label,
    );
    $form['fields'][$field]['title'] = array(
      '#type' => 'textfield',
      '#default_value' => uc_get_field_name($field),
      '#size' => 32,
    );
    $form['fields'][$field]['#weight'] = (isset($weight[$field])) ? $weight[$field] : 99;
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['actions']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to defaults'),
    '#submit' => array('uc_store_address_fields_form_reset'),
  );

  return $form;
}

/**
 * Returns HTML for uc_store_address_fields_form().
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @see uc_store_address_fields_form()
 * @ingroup themeable
 */
function theme_uc_store_address_fields_form($variables) {
  $form = $variables['form'];

  $header = array(t('Field'), t('Title'), t('Enabled'), t('Required'), t('List position'));

  // Sort fields by weight
  uasort($form['fields'], 'element_sort');

  foreach (element_children($form['fields']) as $field) {
    $row = array(
      drupal_render($form['fields'][$field]['default']),
      drupal_render($form['fields'][$field]['title']),
      drupal_render($form['fields'][$field]['enabled']),
      drupal_render($form['fields'][$field]['required']),
      drupal_render($form['fields'][$field]['weight']),
    );

    $rows[] = array(
      'data' => $row,
      'class' => array('draggable'),
    );
  }

  drupal_add_tabledrag('uc-store-address-fields-weight-table', 'order', 'sibling', 'uc-store-address-fields-weight');

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'uc-store-address-fields-weight-table'),
  ));
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Saves the address fields settings.
 *
 * @see uc_store_address_fields_form()
 */
function uc_store_address_fields_form_submit($form, &$form_state) {
  $fields = array();
  $required = array();
  $weight = array();
  foreach ($form_state['values']['fields'] as $field => $data) {
    variable_set('uc_field_' . $field, $data['title']);
    if (isset($data['enabled']) && $data['enabled']) {
      $fields[] = $field;
    }
    if (isset($data['required']) && $data['required']) {
      $required[] = $field;
    }
    if (isset($data['weight'])) {
      $weight[$field] = $data['weight'];
    }
  }
  variable_set('uc_address_fields', drupal_map_assoc($fields));
  variable_set('uc_address_fields_required', drupal_map_assoc($required));
  variable_set('uc_address_fields_weight', $weight);
  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Resets the address fields settings to their default values.
 *
 * @see uc_store_address_fields_form()
 */
function uc_store_address_fields_form_reset($form_id, &$form_state) {
  foreach ($form_state['values']['fields'] as $field => $data) {
    variable_del('uc_field_' . $field);
  }
  variable_del('uc_address_fields');
  variable_del('uc_address_fields_required');
  variable_del('uc_address_fields_weight');
  drupal_set_message(t('The configuration options have been reset to their default values.'));
}

/**
 * Form builder for store settings.
 *
 * @see uc_store_settings_form_validate()
 * @ingroup forms
 */
function uc_store_settings_form($form, &$form_state) {
  $form['store'] = array('#type' => 'vertical_tabs');

  $form['basic'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic information'),
    '#group' => 'store',
  );
  $form['basic']['uc_store_name'] = uc_textfield(t('Store name'), uc_store_name(), FALSE, NULL, 64);
  $form['basic']['uc_store_owner'] = uc_textfield(t('Store owner'), variable_get('uc_store_owner', NULL), FALSE, NULL, 64);
  $form['basic']['uc_store_email'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail address'),
    '#description' => NULL,
    '#size' => 32,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#default_value' => uc_store_email(),
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
    '#weight' => 10,
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
      drupal_set_message(t('!country disabled.', array('!country' => t($country->country_name))));
    }
    else {
      drupal_set_message(t('!country is already disabled.', array('!country' => t($country->country_name))), 'error');
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
      drupal_set_message(t('@country enabled.', array('@country' => t($country->country_name))));
    }
    else {
      drupal_set_message(t('@country is already enabled.', array('@country' => t($country->country_name))), 'error');
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
  $country = t(db_query("SELECT country_name FROM {uc_countries} WHERE country_id = :id", array(':id' => $country_id))->fetchField());

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

  drupal_set_message(t('!country removed.', array('!country' => t($country->country_name))));
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
