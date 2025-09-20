<?php
/**
 * @file
 * UPS administration menu items with OAuth integration.
 */

/**
 * UPS Online Tool settings.
 *
 * Updates the integration to use OAuth authentication instead of legacy authentication methods.
 */
function uc_ups_admin_settings($form, &$form_state) {
  $config = config('uc_ups.settings');
  $form['ups-settings'] = array(
    '#type' => 'vertical_tabs',
  );

  // OAuth Credentials
  $form['uc_ups_credentials'] = array(
    '#type' => 'fieldset',
    '#title' => t('OAuth Credentials'),
    '#description' => t('Enter UPS OAuth credentials.'),
    '#group' => 'ups-settings',
  );

  $form['uc_ups_credentials']['uc_ups_shipper_number'] = array(
    '#type' => 'textfield',
    '#title' => t('UPS Shipper Number'),
    '#default_value' => $config->get('uc_ups_shipper_number'),
    '#required' => TRUE,
  );

  $form['uc_ups_credentials']['uc_ups_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('UPS Client ID'),
    '#default_value' => $config->get('uc_ups_client_id'),
    '#required' => TRUE,
  );

  $client_secret = $config->get('uc_ups_client_secret');
  $form['uc_ups_credentials']['uc_ups_client_secret'] = array(
    '#type' => 'password',
    '#title' => t('UPS Client Secret'),
    '#placeholder' => empty($client_secret) ? t('Enter the client secret') : t('<previous secret saved>'),
  );

  $form['uc_ups_credentials']['uc_ups_access_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Access Token'),
    '#default_value' => state_get('uc_ups_access_token'),
    '#disabled' => TRUE,
    '#description' => t('This token is automatically retrieved via OAuth authentication.'),
  );

  $form['uc_ups_credentials']['uc_ups_connection_address'] = array(
    '#type' => 'select',
    '#title' => t('Server Mode'),
    '#description' => t('Select Testing or Production mode for UPS API connection.'),
    '#options' => array(
      'https://wwwcie.ups.com/' => t('Testing'),
      'https://onlinetools.ups.com/' => t('Production'),
    ),
    '#default_value' => $config->get('uc_ups_connection_address'),
  );

  // Service Options
  $form['services'] = array(
    '#type' => 'fieldset',
    '#title' => t('Service Options'),
    '#description' => t('Set the conditions that will return a UPS quote.'),
    '#group' => 'ups-settings',
  );

  $form['services']['uc_ups_services'] = array(
    '#type' => 'checkboxes',
    '#title' => t('UPS Services'),
    '#default_value' => $config->get('uc_ups_services'),
    '#options' => _uc_ups_service_list(),
    '#description' => t('Select the UPS services that are available to customers.'),
  );

  // Quote Options
  $form['uc_ups_quote_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Quote Options'),
    '#description' => t('Preferences that affect computation of quote.'),
    '#group' => 'ups-settings',
  );

  $form['uc_ups_quote_options']['uc_ups_negotiated_rates'] = array(
    '#type' => 'radios',
    '#title' => t('Negotiated Rates'),
    '#default_value' => $config->get('uc_ups_negotiated_rates'),
    '#options' => array(
      1 => t('Yes'),
      0 => t('No'),
    ),
    '#description' => t('Is your UPS account receiving negotiated rates on shipments?'),
  );

  // Markups
  $form['uc_ups_markups'] = array(
    '#type' => 'fieldset',
    '#title' => t('Markups'),
    '#description' => t('Modifiers to the shipping weight and quoted rate.'),
    '#group' => 'ups-settings',
  );

  $form['uc_ups_markups']['uc_ups_rate_markup_type'] = array(
    '#type' => 'select',
    '#title' => t('Rate Markup Type'),
    '#default_value' => $config->get('uc_ups_rate_markup_type'),
    '#options' => array(
      'percentage' => t('Percentage (%)'),
      'multiplier' => t('Multiplier (×)'),
      'currency' => t('Addition (!currency)', array('!currency' => config_get('uc_store.settings', 'uc_currency_sign'))),
    ),
  );

  $form['uc_ups_markups']['uc_ups_rate_markup'] = array(
    '#type' => 'textfield',
    '#title' => t('Shipping Rate Markup'),
    '#default_value' => $config->get('uc_ups_rate_markup'),
    '#description' => t('Markup shipping rate quote by currency amount, percentage, or multiplier.'),
  );

  // Label Printing
  $form['uc_ups_labels'] = array(
    '#type' => 'fieldset',
    '#title' => t('Label Printing'),
    '#description' => t('Preferences for UPS Shipping Label Printing. Additional permissions from UPS are required to use this feature.'),
    '#group' => 'ups-settings',
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for UPS settings.
 */
function uc_ups_admin_settings_submit($form, &$form_state) {
  $fields = array(
    'uc_ups_shipper_number',
    'uc_ups_client_id',
//    'uc_ups_client_secret',
    'uc_ups_rate_markup',
    'uc_ups_negotiated_rates',
    'uc_ups_services',
    'uc_ups_connection_address',
  );

  foreach ($fields as $key) {
    $value = $form_state['values'][$key];
    if (is_array($value) && isset($form_state['values']['array_filter'])) {
      $value = array_keys(array_filter($value));
    }
    config_set('uc_ups.settings', $key, $value);
  }

  // Update client secret if we entered a value.
  $client_secret = $form_state['values']['uc_ups_client_secret'];
  if (!empty($client_secret)) {
    config_set('uc_ups.settings', 'uc_ups_client_secret', $client_secret);
  }

  // Check token expiration
  $expires_in = state_get('uc_ups_token_expiry');
  $current_time = REQUEST_TIME;

  if ($expires_in > $current_time + 300) {
    backdrop_set_message(t('OAuth token is still valid. No refresh needed. Expires in @time seconds.', ['@time' => $expires_in - $current_time]));
  }
  else {
    if (function_exists('uc_ups_get_access_token')) {
      $token = uc_ups_get_access_token();
      if ($token) {
        backdrop_set_message(t('OAuth token successfully retrieved.'));
      }
      else {
        watchdog('uc_ups', 'Failed to retrieve OAuth token.', [], WATCHDOG_ERROR);
        backdrop_set_message(t('Failed to retrieve OAuth token. Check the logs for details.'), 'error');
      }
    }
  }
}
