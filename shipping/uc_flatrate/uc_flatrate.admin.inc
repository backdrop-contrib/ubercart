<?php

/**
 * @file
 * Flat rate shipping method administration menu items.
 */

/**
 * Lists and compares all flatrate shipping quote methods.
 */
function uc_flatrate_admin_methods() {
  $build = array();
  $rows = array();

  $result = db_query("SELECT mid, title, label, base_rate, product_rate FROM {uc_flatrate_methods}");
  foreach ($result as $method) {
    $row = array();
    $row[] = check_plain($method->title);
    $row[] = check_plain($method->label);
    $row[] = uc_currency_format($method->base_rate);
    $row[] = uc_currency_format($method->product_rate);
    $row[] = l(t('edit'), 'admin/store/settings/quotes/methods/flatrate/' . $method->mid);
    $rows[] = $row;
  }

  if (count($rows)) {
    $header = array(t('Title'), t('Label'), t('Base rate'), t('Default product rate'), array('data' => t('Operations'), 'colspan' => 1));
    $build['methods'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
    );
  }

  return $build;
}

/**
 * Configures the store default product shipping rates.
 *
 * @see uc_flatrate_admin_method_edit_form_validate()
 * @see uc_flatrate_admin_method_edit_form_submit()
 * @see uc_flatrate_admin_method_edit_form_delete()
 * @ingroup forms
 */
function uc_flatrate_admin_method_edit_form($form, &$form_state, $mid = 0) {
  $sign_flag = variable_get('uc_sign_after_amount', FALSE);
  $currency_sign = variable_get('uc_currency_sign', '$');

  if ($mid && ($method = db_query("SELECT * FROM {uc_flatrate_methods} WHERE mid = :mid", array(':mid' => $mid))->fetchObject())) {
    $form['mid'] = array('#type' => 'value', '#value' => $mid);
  }
  else {
    $method = (object) array(
      'title' => '',
      'label' => '',
      'base_rate' => '',
      'product_rate' => '',
    );
  }

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Shipping method title'),
    '#description' => t('The name shown to distinguish it from other flatrate methods.'),
    '#default_value' => $method->title,
    '#required' => TRUE,
  );
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Line item label'),
    '#description' => t('The name shown to the customer when they choose a shipping method at checkout.'),
    '#default_value' => $method->label,
    '#required' => TRUE,
  );
  $form['base_rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Base price'),
    '#description' => t('The starting price for shipping costs.'),
    '#default_value' => $method->base_rate,
    '#required' => TRUE,
    '#size' => 16,
    '#field_prefix' => $sign_flag ? '' : $currency_sign,
    '#field_suffix' => $sign_flag ? $currency_sign : '',
  );
  $form['product_rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Default product shipping rate'),
    '#description' => t('Additional shipping cost per product in cart.'),
    '#default_value' => $method->product_rate,
    '#required' => TRUE,
    '#size' => 16,
    '#field_prefix' => $sign_flag ? '' : $currency_sign,
    '#field_suffix' => $sign_flag ? $currency_sign : '',
  );

  if (module_exists('rules')) {
    $conditions = rules_config_load('get_quote_from_flatrate_' . $mid);
    if ($conditions) {
      RulesPluginUI::$basePath = 'admin/config/workflow/rules/components';
      $conditions->form($form, $form_state);
    }
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  if (isset($form['mid'])) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#validate' => array(),
      '#submit' => array('uc_flatrate_admin_method_edit_form_delete'),
    );
  }

  return $form;
}

/**
 * Form validation for uc_flatrate_admin_method_edit_form().
 *
 * @see uc_flatrate_admin_method_edit_form()
 * @see uc_flatrate_admin_method_edit_form_submit()
 */
function uc_flatrate_admin_method_edit_form_validate($form, &$form_state) {
  if (!empty($form_state['values']['base_rate']) && !is_numeric($form_state['values']['base_rate'])) {
    form_set_error('base_rate', t('The base rate must be a numeric amount.'));
  }
  if (!empty($form_state['values']['product_rate']) && !is_numeric($form_state['values']['product_rate'])) {
    form_set_error('product_rate', t('The product rate must be a numeric amount.'));
  }
}

/**
 * Helper function to delete a flatrate method.
 *
 * @see uc_flatrate_admin_method_edit_form()
 */
function uc_flatrate_admin_method_edit_form_delete($form, &$form_state) {
  drupal_goto('admin/store/settings/quotes/flatrate/' . $form_state['values']['mid'] . '/delete');
}

/**
 * Form submission handler for uc_flatrate_admin_method_edit_form().
 *
 * @see uc_flatrate_admin_method_edit_form()
 * @see uc_flatrate_admin_method_edit_form_validate()
 */
function uc_flatrate_admin_method_edit_form_submit($form, &$form_state) {
  if (isset($form_state['values']['mid'])) {
    drupal_write_record('uc_flatrate_methods', $form_state['values'], 'mid');
    drupal_set_message(t('Flat rate shipping method was updated.'));
  }
  else {
    drupal_write_record('uc_flatrate_methods', $form_state['values']);
    $mid = $form_state['values']['mid'];

    $enabled = variable_get('uc_quote_enabled', array());
    $enabled['flatrate_' . $mid] = TRUE;
    variable_set('uc_quote_enabled', $enabled);

    $weight = variable_get('uc_quote_method_weight', array());
    $weight['flatrate_' . $mid] = 0;
    variable_set('uc_quote_method_weight', $weight);

    // Ensure Rules picks up the new condition.
    if (module_exists('rules')) {
      entity_flush_caches();
    }

    drupal_set_message(t('Created and enabled new flat rate shipping method.'));
  }

  $form_state['redirect'] = 'admin/store/settings/quotes/methods/flatrate';
}

/******************************************************************************
 * Menu Callbacks                                                             *
 ******************************************************************************/

/**
 * Confirms deletion of a flat rate shipping method.
 *
 * @see uc_flatrate_admin_method_confirm_delete_submit()
 * @ingroup forms
 */
function uc_flatrate_admin_method_confirm_delete($form, &$form_state, $mid) {
  $form['mid'] = array('#type' => 'value', '#value' => $mid);

  return confirm_form($form, t('Do you want to delete this shipping method?'),
    'admin/store/settings/quotes/methods/flatrate',
    t('This will remove the shipping method and the product-specific overrides (if applicable). This action can not be undone.'),
    t('Delete'));
}

/**
 * Form submission handler for uc_flatrate_admin_method_confirm_delete().
 *
 * @see uc_flatrate_admin_method_confirm_delete()
 */
function uc_flatrate_admin_method_confirm_delete_submit($form, &$form_state) {
  $mid = $form_state['values']['mid'];

  db_delete('uc_flatrate_methods')
    ->condition('mid', $mid)
    ->execute();
  db_delete('uc_flatrate_products')
    ->condition('mid', $mid)
    ->execute();

  if (module_exists('rules')) {
    rules_config_delete(array('get_quote_from_flatrate_' . $mid));
  }

  $enabled = variable_get('uc_quote_enabled', array());
  unset($enabled['flatrate_' . $mid]);
  variable_set('uc_quote_enabled', $enabled);

  $weight = variable_get('uc_quote_method_weight', array());
  unset($weight['flatrate_' . $mid]);
  variable_set('uc_quote_method_weight', $weight);

  drupal_set_message(t('Flat rate shipping method deleted.'));
  $form_state['redirect'] = 'admin/store/settings/quotes/methods/flatrate';
}
