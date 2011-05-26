<?php

/**
 * @file
 * Shipping quotes administration menu items.
 */

/**
 * Displays an overview of the shipping quote settings.
 */
function uc_quote_overview() {
  $summaries = array();

  // Load the form summaries for pages beneath this path.
  $summaries = array_merge($summaries, uc_summarize_child_form_pages('admin/store/settings/quotes/edit', FALSE, TRUE));
  $summaries = array_merge($summaries, uc_summarize_child_form_pages('admin/store/settings/quotes/methods', FALSE, TRUE));

  // Theme it all up in a summaries overview.
  return array(
    '#theme' => 'uc_summary_overview',
    '#summaries' => $summaries,
  );
}

/**
 * Default shipping settings.
 *
 * Sets the default shipping location of the store. Allows the user to
 * determine which quotin methods are enabled and which take precedence over
 * the others. Also sets the default quote and shipping types of all products
 * in the store. Individual products may be configured differently.
 *
 * @see uc_quote_admin_settings_submit()
 * @ingroup forms
 */
function uc_quote_admin_settings($form, &$form_state) {
  $address = variable_get('uc_quote_store_default_address', new UcAddress());

  $form['uc_quote_log_errors'] = array(
    '#type' => 'checkbox',
    '#title' => t('Log errors during checkout to watchdog'),
    '#summary callback' => 'uc_summarize_checkbox',
    '#summary arguments' => array(
      t('Quote errors are submitted to watchdog.'),
      t('Quote errors are not submitted to watchdog.'),
    ),
    '#default_value' => variable_get('uc_quote_log_errors', FALSE),
  );
  $form['uc_quote_display_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display debug information to administrators.'),
    '#summary callback' => 'uc_summarize_checkbox',
    '#summary arguments' => array(
      t('Debugging information is displayed to administrators when quotes are generated.'),
      t('Debugging information is not displayed to administrators when quotes are generated.'),
    ),
    '#default_value' => variable_get('uc_quote_display_debug', FALSE),
  );
  $form['uc_quote_require_quote'] = array(
    '#type' => 'checkbox',
    '#title' => t('Prevent the customer from completing an order if a shipping quote is not selected.'),
    '#summary callback' => 'uc_summarize_checkbox',
    '#summary arguments' => array(
      t('Customers cannot complete checkout without selecting a shipping quote.'),
      t('Customers can still checkout without selecting a shipping quote.'),
    ),
    '#default_value' => variable_get('uc_quote_require_quote', TRUE),
  );

  $form['uc_quote_pane_description'] = array(
    '#type' => 'fieldset',
    '#title' => t('Shipping quote pane description'),
    '#summary callback ' => 'uc_summarize_form',
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['uc_quote_pane_description']['text'] = array(
    '#type' => 'textarea',
    '#title' => t('Message text'),
    '#default_value' => variable_get('uc_quote_pane_description', t('Shipping quotes are generated automatically when you enter your address and may be updated manually with the button below.')),
  );

  $form['uc_quote_err_msg'] = array(
    '#type' => 'fieldset',
    '#title' => t('Shipping quote error message'),
    '#summary callback' => 'uc_summarize_form',
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['uc_quote_err_msg']['text'] = array(
    '#type' => 'textarea',
    '#title' => t('Message text'),
    '#default_value' => variable_get('uc_quote_err_msg', t("There were problems getting a shipping quote. Please verify the delivery and product information and try again.\nIf this does not resolve the issue, please call in to complete your order.")),
  );

  $form['default_address'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default pickup address'),
    '#description' => t("When delivering products to customers, the original location of the product must be known in order to accurately quote the shipping cost and set up a delivery. This form provides the default location for products across the entire store. If a product's individual pickup address is blank, Ubercart looks for the manufacturer's. If that is also blank, it uses the store's default pickup address."),
    '#summary' => t('Default pickup address is: <br />!address', array('!address' => uc_address_format(
      $address->first_name,
      $address->last_name,
      $address->company,
      $address->street1,
      $address->street2,
      $address->city,
      $address->zone,
      $address->postal_code,
      $address->country
    ))),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['default_address']['address'] = array(
    '#type' => 'uc_address',
    '#default_value' => isset($form_state['values']) ? $form_state['values'] : $address,
    '#required' => FALSE,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save configuration') );
  return $form;
}

/**
 * Form submission handler for uc_quote_admin_settings().
 *
 * @see uc_quote_admin_settings()
 */
function uc_quote_admin_settings_submit($form, &$form_state) {
  $address = new UcAddress();
  $address->first_name = $form_state['values']['first_name'];
  $address->last_name = $form_state['values']['last_name'];
  $address->company = $form_state['values']['company'];
  $address->phone = $form_state['values']['phone'];
  $address->street1 = $form_state['values']['street1'];
  $address->street2 = $form_state['values']['street2'];
  $address->city = $form_state['values']['city'];
  $address->zone = $form_state['values']['zone'];
  $address->postal_code = $form_state['values']['postal_code'];
  $address->country = $form_state['values']['country'];

  variable_set('uc_quote_store_default_address', $address);
  variable_set('uc_quote_log_errors', $form_state['values']['uc_quote_log_errors']);
  variable_set('uc_quote_display_debug', $form_state['values']['uc_quote_display_debug']);
  variable_set('uc_quote_require_quote', $form_state['values']['uc_quote_require_quote']);
  variable_set('uc_quote_pane_description', $form_state['values']['uc_quote_pane_description']['text']);
  variable_set('uc_quote_err_msg', $form_state['values']['uc_quote_err_msg']['text']);

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Settings for the shipping quote methods.
 *
 * Enable and reorder shipping quote methods. Set the default shipping type.
 *
 * @see uc_quote_method_settings_validate()
 * @see uc_quote_method_settings_submit()
 * @see theme_uc_quote_method_settings()
 * @ingroup forms
 */
function uc_quote_method_settings($form, &$form_state) {
  $form['methods'] = array(
    '#tree' => TRUE,
    '#summary callback' => 'uc_summarize_form',
  );

  $enabled = variable_get('uc_quote_enabled', array());
  $weight = variable_get('uc_quote_method_weight', array());
  $methods = uc_quote_shipping_method_options();
  if (is_array($methods)) {
    foreach ($methods as $id => $title) {
      // Ensure default values are set.
      $enabled += array($id => FALSE);
      $weight += array($id => 0);

      $form['methods'][$id]['#summary callback'] = 'uc_summarize_form';
      $form['methods'][$id]['uc_quote_enabled'] = array('#type' => 'checkbox',
        '#default_value' => $enabled[$id],
        '#summary callback' => 'uc_summarize_checkbox',
        '#summary arguments' => array(
          l(t('@method is enabled.', array('@method' => $title)), 'admin/store/settings/quotes/methods/' . $id),
          '',
        ),
      );
      $form['methods'][$id]['uc_quote_method_weight'] = array('#type' => 'weight',
        '#delta' => 5,
        '#default_value' => $weight[$id],
      );
    }
  }

  $shipping_types = uc_quote_shipping_type_options();
  if (is_array($shipping_types)) {
    $form['uc_quote_type_weight'] = array('#type' => 'fieldset',
      '#title' => t('List position'),
      '#description' => t('Determines which shipping methods are quoted at checkout when products of different shipping types are ordered. Larger values take precedence.'),
      '#collapsible' => TRUE,
      '#summary callback' => 'uc_summarize_null',
      '#tree' => TRUE,
    );
    $weight = variable_get('uc_quote_type_weight', array());
    $shipping_methods = module_invoke_all('uc_shipping_method');
    $method_types = array();
    foreach ($shipping_methods as $method) {
      $method_types[$method['quote']['type']][] = $method['title'];
    }
    if (isset($method_types['order']) && is_array($method_types['order'])) {
      $count = count($method_types['order']);
      $form['uc_quote_type_weight']['#description'] .= format_plural($count, '<br />The %list method is compatible with any shipping type.', '<br />The %list methods are compatible with any shipping type.', array('%list' => implode(', ', $method_types['order'])));
    }
    foreach ($shipping_types as $id => $title) {
      $form['uc_quote_type_weight'][$id] = array('#type' => 'weight',
        '#title' => $title . (isset($method_types[$id]) && is_array($method_types[$id]) ? ' (' . implode(', ', $method_types[$id]) . ')' : ''),
        '#delta' => 5,
        '#default_value' => isset($weight[$id]) ? $weight[$id] : 0,
      );
    }
  }
  $form['uc_store_shipping_type'] = array('#type' => 'select',
    '#title' => t('Default order fulfillment type for products'),
    '#options' => $shipping_types,
    '#default_value' => variable_get('uc_store_shipping_type', 'small_package'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save configuration') );

  return $form;
}

/**
 * Displays a formatted list of shipping quote methods and form elements.
 *
 * @see uc_quote_method_settings()
 * @ingroup themeable
 */
function theme_uc_quote_method_settings($variables) {
  $form = $variables['form'];

  $methods = uc_quote_shipping_method_options();
  $output = '';

  $header = array(t('Enable'), t('Shipping method'), t('List position'));
  $rows = array();
  foreach (element_children($form['methods']) as $method) {
    $row = array();
    $row[] = drupal_render($form['methods'][$method]['uc_quote_enabled']);
    $row[] = $methods[$method];
    $row[] = drupal_render($form['methods'][$method]['uc_quote_method_weight']);
    $rows[] = $row;
  }
  $output .= theme('table', array('header' => $header, 'rows' => $rows));

  $output .= drupal_render_children($form);
  return $output;
}

/**
 * Form validation for uc_quote_method_settinggs().
 *
 * Requires at least one enabled shipping method.
 *
 * @see uc_quote_method_settings()
 * @see uc_quote_method_settings_submit()
 */
function uc_quote_method_settings_validate($form, &$form_state) {
  $none_enabled = TRUE;
  if (is_array($form_state['values']['methods'])) {
    foreach ($form_state['values']['methods'] as $method) {
      if ($method['uc_quote_enabled']) {
        $none_enabled = FALSE;
      }
    }
  }
  if ($none_enabled) {
    form_set_error('uc_quote_enabled', t('At least one shipping quote method must be enabled.'));
  }
}

/**
 * Form submission handler for uc_quote_method_settinggs().
 *
 * @see uc_quote_method_settings()
 * @see uc_quote_method_settings_validate()
 */
function uc_quote_method_settings_submit($form, &$form_state) {
  $enabled = array();
  $method_weight = array();
  foreach ($form_state['values']['methods'] as $id => $method) {
    $enabled[$id] = $method['uc_quote_enabled'];
    $method_weight[$id] = $method['uc_quote_method_weight'];
  }

  variable_set('uc_quote_enabled', $enabled);
  variable_set('uc_quote_method_weight', $method_weight);
  variable_set('uc_quote_type_weight', $form_state['values']['uc_quote_type_weight']);
  variable_set('uc_store_shipping_type', $form_state['values']['uc_store_shipping_type']);
  drupal_set_message(t('The configuration options have been saved.'));
}
