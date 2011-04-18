<?php

/**
 * @file
 * Taxes administration menu items.
 */

/**
 * Displays a list of tax rates.
 */
function uc_taxes_admin_settings() {
  $rows = array();

  $header = array(t('Name'), t('Rate'), t('Taxed products'), t('Taxed product types'), t('Taxed line items'), t('Weight'), array('data' => t('Operations'), 'colspan' => 3));

  // Loop through all the defined tax rates.
  foreach (uc_taxes_rate_load() as $rate_id => $rate) {
    // Build the operations links for the tax rate.

    // Add the row to the table.
    $rows[] = array(
      check_plain($rate->name),
      $rate->rate * 100 . '%',
      $rate->shippable ? t('Shippable products') : t('Any product'),
      implode(', ', $rate->taxed_product_types),
      implode(', ', $rate->taxed_line_items),
      $rate->weight,
      l(t('edit'), 'admin/store/settings/taxes/' . $rate_id . '/edit'),
      l(t('clone'), 'admin/store/settings/taxes/' . $rate_id . '/clone'),
      l(t('delete'), 'admin/store/settings/taxes/' . $rate_id . '/delete'),
    );
  }

  // Let the user know if no tax rates are defined.
  if (empty($rows)) {
    $rows[] = array(array('data' => t('No rates available.'), 'colspan' => 9));
  }

  // Build the output including the table and a link to add a new rate.
  $build['taxes'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );

  return $build;
}

/**
 * Builds a form to add or edit a tax rate.
 *
 * @param $rate_id
 *   The ID of the tax rate to edit; leave NULL to add a new rate.
 *
 * @see uc_taxes_form_validate()
 * @see uc_taxes_form_submit()
 * @ingroup forms
 */
function uc_taxes_form($form, &$form_state, $rate_id = NULL) {
  // If a rate ID was specified...
  if ($rate_id) {
    // Load the tax rate and set the page title.
    $rate = uc_taxes_rate_load($rate_id);

    drupal_set_title($rate->name);
  }

  $form['id'] = array(
    '#type' => 'value',
    '#value' => $rate_id,
  );

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('This name will appear to the customer when this tax is applied to an order.<br />It will also be used to name the corresponding predicate for this rate.'),
    '#default_value' => $rate_id ? $rate->name : '',
    '#required' => TRUE,
  );

  $form['rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Rate'),
    '#description' => t('The tax rate as a percent or decimal. Examples: 6%, .06'),
    '#default_value' => $rate_id ? $rate->rate : '',
    '#size' => 15,
    '#required' => TRUE,
  );

  $form['shippable'] = array(
    '#type' => 'radios',
    '#title' => t('Taxed products'),
    '#options' => array(
      t('Apply tax to any product regardless of its shippability.'),
      t('Apply tax to shippable products only.'),
    ),
    '#default_value' => $rate_id ? $rate->shippable : 0,
  );

  // TODO: Remove the need for a special case for product kit module.
  $options = uc_product_type_names();
  unset($options['product_kit']);
  $options['blank-line'] = t('"Blank line" product');

  $form['taxed_product_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Taxed product types'),
    '#description' => t('Apply taxes to the specified product types/classes.'),
    '#options' => $options,
    '#default_value' => $rate_id ? $rate->taxed_product_types : array(),
  );

  $options = array();

  foreach (_uc_line_item_list() as $line_item) {
    if (!in_array($line_item['id'], array('subtotal', 'tax_subtotal', 'total'))) {
      $options[$line_item['id']] = $line_item['title'];
    }
  }

  $form['taxed_line_items'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Taxed line items'),
    '#description' => t('Adds the checked line item types to the total before applying this tax.'),
    '#options' => $options,
    '#default_value' => $rate_id ? $rate->taxed_line_items : array(),
  );

  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#description' => t('Taxes are sorted by weight and then applied to the order sequentially. This value is important when taxes need to include other tax line items.'),
    '#default_value' => $rate_id ? $rate->weight : 0,
  );

  $form['display_include'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include this tax when displaying product prices.'),
    '#default_value' => $rate_id ? $rate->display_include : 0,
  );

  $form['inclusion_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Tax inclusion text'),
    '#description' => t('This text will be displayed near the price to indicate that it includes tax.'),
    '#default_value' => $rate_id ? $rate->inclusion_text : '',
  );

  if (module_exists('rules')) {
    $conditions = rules_config_load('uc_taxes_' . $rate_id);
    if ($conditions) {
      RulesPluginUI::$basePath = 'admin/config/workflow/rules/components';
      $conditions->form($form, $form_state);
    }
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#suffix' => l(t('Cancel'), 'admin/store/settings/taxes'),
  );

  return $form;
}

/**
 * Ensures that tax rates are positive numbers.
 *
 * @see uc_taxes_form()
 * @see uc_taxes_form_submit()
 */
function uc_taxes_form_validate($form, &$form_state) {
  if (!empty($form_state['values']['rate']) && (floatval($form_state['values']['rate']) < 0)) {
    form_set_error('rate', t('Rate must be a positive number. No commas and only one decimal point.'));
  }
}

/**
 * Form submission handler for uc_taxes_form().
 *
 * @see uc_taxes_form()
 * @see uc_taxes_form_validate()
 */
function uc_taxes_form_submit($form, &$form_state) {
  // Determine the decimal rate value.
  if (strpos($form_state['values']['rate'], '%')) {
    $form_state['values']['rate'] = floatval($form_state['values']['rate']) / 100;
  }
  else {
    $form_state['values']['rate'] = floatval($form_state['values']['rate']);
  }

  // Build the rate object based on the form values and save it.
  $rate = array(
    'id' => $form_state['values']['id'],
    'name' => $form_state['values']['name'],
    'rate' => $form_state['values']['rate'],
    'taxed_product_types' => array_filter($form_state['values']['taxed_product_types']),
    'taxed_line_items' => array_filter($form_state['values']['taxed_line_items']),
    'weight' => $form_state['values']['weight'],
    'shippable' => $form_state['values']['shippable'],
    'display_include' => $form_state['values']['display_include'],
    'inclusion_text' => $form_state['values']['inclusion_text'],
  );
  uc_taxes_rate_save((object) $rate);

  // Update the name of the associated conditions.
  if (module_exists('rules')) {
    $conditions = rules_config_load('uc_taxes_' . $rate['id']);
    if ($conditions) {
      $conditions->label = $rate['name'];
      $conditions->save();
    }
  }

  // Display a message and redirect back to the overview.
  drupal_set_message(t('Tax rate %name saved.', array('%name' => $form_state['values']['name'])));

  $form_state['redirect'] = 'admin/store/settings/taxes';
}

/**
 * Clones a tax rate.
 */
function uc_taxes_clone($rate_id) {
  // Load the source rate object.
  $rate = uc_taxes_rate_load($rate_id);
  $name = $rate->name;

  // Tweak the name and unset the rate ID.
  $rate->name = t('Copy of !name', array('!name' => $rate->name));
  $rate->id = NULL;

  // Save the new rate.
  $rate = uc_taxes_rate_save($rate);

  // Clone the associated conditions as well.
  if (module_exists('rules') && $conditions = rules_config_load('uc_taxes_' . $rate_id)) {
    unset($conditions->id);
    $conditions->save('uc_taxes_' . $rate->id);
  }

  // Display a message and redirect back to the overview.
  drupal_set_message(t('Tax rate %name cloned.', array('%name' => $name)));

  drupal_goto('admin/store/settings/taxes');
}

/**
 * Deletes a tax rule.
 *
 * @see uc_taxes_delete_form_submit()
 * @ingroup forms
 */
function uc_taxes_delete_form($form, &$form_state, $rate_id) {
  // Bail if we got a bad rate ID.
  if (!$rate = uc_taxes_rate_load($rate_id)) {
    drupal_set_message(t('That tax rate does not exist.'), 'error');
    drupal_goto('admin/store/settings/taxes');
  }

  $form['rate_id'] = array(
    '#type' => 'value',
    '#value' => $rate_id,
  );
  $form['name'] = array(
    '#type' => 'value',
    '#value' => $rate->name,
  );

  return confirm_form($form, t('Are you sure you want to delete @name?', array('@name' => $rate->name)), 'admin/store/settings/taxes', t('This action cannot be undone. Any orders that have been charged this tax may lose tax if you proceed.<br />If you just want this tax to no longer be applied to orders, consider disabling its predicate instead.'), t('Delete'), t('Cancel'));
}

/**
 * Form submission handler for uc_taxes_delete_form().
 *
 * @see uc_taxes_delete_form()
 */
function uc_taxes_delete_form_submit($form, &$form_state) {
  // Delete the tax rate.
  uc_taxes_rate_delete($form_state['values']['rate_id']);

  // Display a message and redirect back to the overview.
  drupal_set_message(t('Tax rate %name deleted.', array('%name' => $form_state['values']['name'])));

  $form_state['redirect'] = 'admin/store/settings/taxes';
}
