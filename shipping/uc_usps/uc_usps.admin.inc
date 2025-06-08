<?php
/**
 * @file
 * USPS administration menu items.
 */

/**
 * Configures USPS settings.
 *
 * @see uc_usps_admin_settings_validate()
 * @see uc_usps_admin_settings_submit()
 * @ingroup forms
 */
function uc_usps_admin_settings($form, &$form_state) {
  $config = config('uc_usps.settings');

  // Put fieldsets into vertical tabs.
  $form['usps-settings'] = array(
    '#type' => 'vertical_tabs',
    '#attached' => array(
      'js' => array(
        'vertical-tabs' => backdrop_get_path('module', 'uc_usps') . '/uc_usps.admin.js',
      ),
    ),
  );

  // Container for credential fields.
  $form['uc_usps_credentials'] = array(
    '#type' => 'fieldset',
    '#title' => t('Credentials'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'usps-settings',
  );

  $options = array(
    'legacy' => t('Legacy'),
    'newapi' => t('New API'),
  );
  $form['uc_usps_credentials']['uc_usps_authorization_method'] = array(
    '#type' => 'radios',
    '#title' => t('Method'),
    '#description' => t('Select the method for authorization with USPS. Legacy credentials (user ID, used with the USPS Web Tools API) will be discontinued on 2026-01-25. You should update to the new API and OAuth2 credentials before then. <a href="https://postalpro.usps.com/node/13834">More details here.</a>'),
    '#options' => $options,
    '#default_value' => $config->get('uc_usps_authorization_method'),
  );
  $form['uc_usps_credentials']['legacy'] = array(
    '#type' => 'fieldset',
    '#states' => array(
      'visible' => array(
        ':input[name="uc_usps_authorization_method"]' => array('value' => 'legacy'),
      ),
    ),
  );
  $form['uc_usps_credentials']['legacy']['uc_usps_user_id'] = array(
    '#type' => 'textfield',
    '#title' => t('USPS user ID'),
    '#description' => t('To acquire or locate your user ID, refer to the <a href="!url">USPS documentation</a>.', array('!url' => 'http://drupal.org/node/1308256')),
    '#default_value' => $config->get('uc_usps_user_id'),
  );
  $form['uc_usps_credentials']['newapi'] = array(
    '#type' => 'fieldset',
    '#states' => array(
      'visible' => array(
        ':input[name="uc_usps_authorization_method"]' => array('value' => 'newapi'),
      ),
    ),
  );
  $form['uc_usps_credentials']['newapi']['uc_usps_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('USPS client ID'),
    '#description' => t('Enter your Client ID for OAuth2 authorization.'),
    '#default_value' => $config->get('uc_usps_client_id'),
    '#size' => 120,
  );
  $client_secret = $config->get('uc_usps_client_secret');
  $form['uc_usps_credentials']['newapi']['uc_usps_client_secret'] = array(
    '#type' => 'password',
    '#title' => t('USPS client secret'),
    '#description' => empty($client_secret) ?
      t('Enter your Client Secret for token generation. It will be saved when you save settings but will not be shown after saving for security purposes.') :
      t('You can update your Client Secret here if needed, but if the previously saved value is correct, you do not have to enter anything.'),
    '#placeholder' => empty($client_secret) ? t('Enter the client secret') : t('<previous secret saved>'),
    '#size' => 120,
  );
  $form['uc_usps_credentials']['newapi']['uc_usps_connection_address'] = array(
    '#type' => 'select',
    '#title' => t('Connection address'),
    '#description' => t('Select Testing or Production mode for USPS API connection.'),
    '#options' => array(
      'https://apis.usps.com' => t('Production'),
      'https://apis-tem.usps.com' => t('Testing'),
    ),
    '#default_value' => $config->get('uc_usps_connection_address'),
  );
  $access_token = uc_usps_access_token();
  $token_expiry = new DateTime('@' . $config->get('uc_usps_token_expiry'));
  global $user;
  $token_expiry->setTimeZone(new DateTimeZone($user->timezone));
  $form['uc_usps_credentials']['newapi']['uc_usps_access_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Access token'),
    '#description' => t('This token is automatically retrieved via OAuth authentication. The current token expires @expiry but will be renewed as needed.', array('@expiry' => date_format($token_expiry, 'F j, g:i a T'))),
    '#default_value' => $access_token,
    '#disabled' => TRUE,
    '#size' => 120,
  );

  // Container for domestic quotes
  $form['domestic'] = array(
    '#type' => 'fieldset',
    '#title' => t('USPS domestic'),
    '#description' => t('Set the conditions that will return a USPS domestic quote.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'usps-settings',
  );

  $form['domestic']['legacy'] = array(
    '#type' => 'fieldset',
    '#states' => array(
      'visible' => array(
        ':input[name="uc_usps_authorization_method"]' => array('value' => 'legacy'),
      ),
    ),
  );
  $form['domestic']['legacy']['uc_usps_online_rates'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display USPS "online" rates'),
    '#default_value' => $config->get('uc_usps_online_rates'),
    '#description' => t('Show your customer standard USPS rates (default) or discounted "online" rates.  Online rates apply only if you, the merchant, pay for and print out postage from the USPS <a href="https://cns.usps.com/labelInformation.shtml">Click-N-Ship</a> web site.'),
  );
  $form['domestic']['legacy']['uc_usps_env_services'] = array(
    '#type' => 'checkboxes',
    '#title' => t('USPS envelope services'),
    '#options' => _uc_usps_env_services(),
    '#default_value' => $config->get('uc_usps_env_services'),
    '#description' => t('Select the USPS services that are available to customers. Be sure to include the services that the Postal Service agrees are available to you.'),
  );

  $form['domestic']['legacy']['uc_usps_services'] = array(
    '#type' => 'checkboxes',
    '#title' => t('USPS small package services'),
    '#options' => _uc_usps_services(),
    '#default_value' => $config->get('uc_usps_services'),
    '#description' => t('Select the USPS services that are available to customers. Be sure to include the services that the Postal Service agrees are available to you.'),
  );
  $form['domestic']['newapi'] = array(
    '#type' => 'fieldset',
    '#states' => array(
      'visible' => array(
        ':input[name="uc_usps_authorization_method"]' => array('value' => 'newapi'),
      ),
    ),
  );
  $form['domestic']['newapi']['uc_usps_newapi_dom_env_services'] = array(
    '#type' => 'checkboxes',
    '#title' => t('USPS envelope services'),
    '#options' => $config->get('uc_usps_newapi_dom_env_all_services'),
    '#default_value' => $config->get('uc_usps_newapi_dom_env_services'),
    '#description' => t('Select the USPS services that are available to customers. Be sure to include the services that the Postal Service agrees are available to you.'),
  );

  $form['domestic']['newapi']['uc_usps_newapi_dom_parcel_services'] = array(
    '#type' => 'checkboxes',
    '#title' => t('USPS small package services'),
    '#options' => $config->get('uc_usps_newapi_dom_parcel_all_services'),
    '#default_value' => $config->get('uc_usps_newapi_dom_parcel_services'),
    '#description' => t('Select the USPS services that are available to customers. Be sure to include the services that the Postal Service agrees are available to you.'),
  );

  // Container for international quotes
  $form['international'] = array(
    '#type' => 'fieldset',
    '#title' => t('USPS international'),
    '#description' => t('Set the conditions that will return a USPS international quote.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'usps-settings',
  );
  $form['international']['legacy'] = array(
    '#type' => 'fieldset',
    '#states' => array(
      'visible' => array(
        ':input[name="uc_usps_authorization_method"]' => array('value' => 'legacy'),
      ),
    ),
  );
  $form['international']['legacy']['uc_usps_intl_env_services'] = array(
    '#type' => 'checkboxes',
    '#title' => t('USPS international envelope services'),
    '#options' => _uc_usps_intl_env_services(),
    '#default_value' => $config->get('uc_usps_intl_env_services'),
    '#description' => t('Select the USPS services that are available to customers. Be sure to include the services that the Postal Service agrees are available to you.'),
  );

  $form['international']['legacy']['uc_usps_intl_services'] = array(
    '#type' => 'checkboxes',
    '#title' => t('USPS international parcel services'),
    '#options' => _uc_usps_intl_services(),
    '#default_value' => $config->get('uc_usps_intl_services'),
    '#description' => t('Select the USPS services that are available to customers. Be sure to include the services that the Postal Service agrees are available to you.'),
  );
  $form['international']['newapi'] = array(
    '#type' => 'fieldset',
    '#states' => array(
      'visible' => array(
        ':input[name="uc_usps_authorization_method"]' => array('value' => 'newapi'),
      ),
    ),
  );
  $form['international']['newapi']['uc_usps_newapi_intl_env_services'] = array(
    '#type' => 'checkboxes',
    '#title' => t('USPS international envelope services'),
    '#options' => $config->get('uc_usps_newapi_intl_env_all_services'),
    '#default_value' => $config->get('uc_usps_newapi_intl_env_services'),
    '#description' => t('Select the USPS services that are available to customers. Be sure to include the services that the Postal Service agrees are available to you.'),
  );
  $form['international']['newapi']['uc_usps_newapi_intl_parcel_services'] = array(
    '#type' => 'checkboxes',
    '#title' => t('USPS international parcel services'),
    '#options' => $config->get('uc_usps_newapi_intl_parcel_all_services'),
    '#default_value' => $config->get('uc_usps_newapi_intl_parcel_services'),
    '#description' => t('Select the USPS services that are available to customers. Be sure to include the services that the Postal Service agrees are available to you.'),
  );

  // Container for quote options.
  $form['uc_usps_quote_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Quote options'),
    '#description' => t('Preferences that affect computation of quote.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'usps-settings',
  );

  // All in one
  $form['uc_usps_quote_options']['uc_usps_all_in_one'] = array(
    '#type' => 'radios',
    '#title' => t('Product packages'),
    '#default_value' => $config->get('uc_usps_all_in_one'),
    '#options' => array(
      0 => t('Each product in its own package'),
      1 => t('All products in one package'),
    ),
    '#description' => t('Indicate whether each product is quoted as shipping separately or all in one package. Orders with one kind of product will still use the package quantity to determine the number of packages needed, however.'),
  );
  // Price type (New API only).
  $form['uc_usps_quote_options']['uc_usps_price_type'] = array(
    '#type' => 'select',
    '#title' => t('Price type'),
    '#description' => t('Select the price type for the price quote from USPS.'),
    '#default_value' => $config->get('uc_usps_price_type'),
    '#options' => _uc_usps_newapi_price_types(),
    '#states' => array(
      'visible' => array(
        ':input[name="uc_usps_authorization_method"]' => array('value' => 'newapi'),
      ),
    ),
  );
  // Insurance.
  $form['uc_usps_quote_options']['uc_usps_insurance'] = array(
    '#type' => 'checkbox',
    '#title' => t('Package insurance'),
    '#default_value' => $config->get('uc_usps_insurance'),
    '#description' => t('When enabled, the quotes presented to the customer will include the cost of insurance for the full sales price of all products in the order.'),
  );
  // Delivery confirmation (legacy only)
  $form['uc_usps_quote_options']['uc_usps_delivery_confirmation'] = array(
    '#type' => 'checkbox',
    '#title' => t('Delivery confirmation'),
    '#default_value' => $config->get('uc_usps_delivery_confirmation'),
    '#description' => t('When enabled, the quotes presented to the customer will include the cost of delivery confirmation for all packages in the order.'),
    '#states' => array(
      'visible' => array(
        ':input[name="uc_usps_authorization_method"]' => array('value' => 'legacy'),
      ),
    ),
  );
  // Signature confirmation.
  $form['uc_usps_quote_options']['uc_usps_signature_confirmation'] = array(
    '#type' => 'checkbox',
    '#title' => t('Signature confirmation'),
    '#default_value' => $config->get('uc_usps_signature_confirmation'),
    '#description' => t('When enabled, the quotes presented to the customer will include the cost of signature confirmation for all packages in the order.'),
  );
  // Container for markup fields.
  $form['uc_usps_markups'] = array(
    '#type' => 'fieldset',
    '#title' => t('Markups'),
    '#description' => t('Modifiers to the shipping weight and quoted rate.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'usps-settings',
  );

  $form['uc_usps_markups']['uc_usps_weight_markup_type'] = array(
    '#type' => 'select',
    '#title' => t('Weight markup type'),
    '#default_value' => $config->get('uc_usps_weight_markup_type'),
    '#options' => array(
      'percentage' => t('Percentage (%)'),
      'multiplier' => t('Multiplier (×)'),
      'mass' => t('Addition (!mass)', array('!mass' => '#')),
    ),
    '#disabled' => TRUE,
  );
  $form['uc_usps_markups']['uc_usps_weight_markup'] = array(
    '#type' => 'textfield',
    '#title' => t('Shipping weight markup'),
    '#default_value' => $config->get('uc_usps_weight_markup'),
    '#description' => t('Markup shipping weight on a per-package basis before quote, by weight amount, percentage, or multiplier.'),
    '#disabled' => TRUE,
  );

  $form['uc_usps_markups']['uc_usps_rate_markup_type'] = array(
    '#type' => 'select',
    '#title' => t('Rate markup type'),
    '#default_value' => $config->get('uc_usps_rate_markup_type'),
    '#options' => array(
      'percentage' => t('Percentage (%)'),
      'multiplier' => t('Multiplier (×)'),
      'currency' => t('Addition (!currency)', array('!currency' => config_get('uc_store.settings', 'uc_currency_sign'))),
    ),
  );
  $form['uc_usps_markups']['uc_usps_rate_markup'] = array(
    '#type' => 'textfield',
    '#title' => t('Shipping rate markup'),
    '#default_value' => $config->get('uc_usps_rate_markup'),
    '#description' => t('Markup shipping rate quote by dollar amount, percentage, or multiplier.'),
  );

  // Taken from system_settings_form(). Only, don't use its submit handler.
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['actions']['cancel'] = array(
    '#markup' => l(t('Cancel'), 'admin/store/settings/quotes'),
  );

  if (!empty($_POST) && form_get_errors()) {
    backdrop_set_message(t('The settings have not been saved because of the errors.'), 'error');
  }
  if (!isset($form['#theme'])) {
    $form['#theme'] = 'system_settings_form';
  }

  return $form;
}

/**
 * Validation handler for uc_usps_admin_settings form.
 *
 * @see uc_usps_admin_settings()
 * @see uc_usps_admin_settings_submit()
 */
function uc_usps_admin_settings_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['uc_usps_rate_markup'])) {
    form_set_error('uc_usps_rate_markup', t('Rate markup must be a numeric value.'));
  }
  if (!is_numeric($form_state['values']['uc_usps_weight_markup'])) {
    form_set_error('uc_usps_weight_markup', t('Weight markup must be a numeric value.'));
  }
}

/**
 * Submit handler for uc_usps_admin_settings form.
 *
 * @see uc_usps_admin_settings()
 * @see uc_usps_admin_settings_validate()
 */
function uc_usps_admin_settings_submit($form, &$form_state) {
  $config = config('uc_usps.settings');

  // Record whether the testing/production server changed before we save new
  // values.
  $server_changed = $form_state['values']['uc_usps_connection_address'] != $config->get('uc_usps_connection_address');

  $fields = array(
    'uc_usps_authorization_method',
    'uc_usps_user_id',
    'uc_usps_client_id',
    'uc_usps_connection_address',
    'uc_usps_online_rates',
    'uc_usps_env_services',
    'uc_usps_services',
    'uc_usps_intl_env_services',
    'uc_usps_intl_services',
    'uc_usps_newapi_dom_env_services',
    'uc_usps_newapi_dom_parcel_services',
    'uc_usps_newapi_intl_env_services',
    'uc_usps_newapi_intl_parcel_services',
    'uc_usps_price_type',
    'uc_usps_rate_markup_type',
    'uc_usps_rate_markup',
    'uc_usps_weight_markup_type',
    'uc_usps_weight_markup',
    'uc_usps_all_in_one',
    'uc_usps_insurance',
    'uc_usps_delivery_confirmation',
    'uc_usps_signature_confirmation',
  );

  foreach ($fields as $key) {
    $value = $form_state['values'][$key];
    if (is_array($value) && isset($form_state['values']['array_filter'])) {
      $value = array_keys(array_filter($value));
    }
    $config->set($key, $value);
  }

  // Update OAuth2 client secret if we entered a value.
  $client_secret = $form_state['values']['uc_usps_client_secret'];
  if (!empty($client_secret)) {
    $config->set('uc_usps_client_secret', $client_secret);
  }
  // Save all config changes now because _uc_usps_fetch_access_token() will also
  // modify the config file.
  $config->save();
  backdrop_set_message(t('The configuration options have been saved.'));

  // Check token expiration if we're using the new API.
  if ($form_state['values']['uc_usps_authorization_method'] == 'newapi') {
    $token_expiry = $config->get('uc_usps_token_expiry');
    $request_time = REQUEST_TIME;
    $cron_interval = config_get('system.core', 'cron_safe_threshold');
    // Get a new token if the current one will expire before the next cron run
    // or if the server changed.
    if (($token_expiry < $request_time + $cron_interval) || $server_changed) {
      $token = _uc_usps_fetch_access_token();
      if ($token) {
        backdrop_set_message(t('OAuth token successfully retrieved.'));
      }
      else {
        watchdog('uc_usps', 'Failed to retrieve OAuth token.', [], WATCHDOG_ERROR);
        backdrop_set_message(t('Failed to retrieve OAuth token. Check the logs for details.'), 'error');
      }
    }
    else {
      backdrop_set_message(t('OAuth token is still valid. No refresh needed. Expires in @time minutes.', array('@time' => round(($token_expiry - $request_time) / 60))));
    }
  }

  cache_clear_all();
  backdrop_theme_rebuild();
}
