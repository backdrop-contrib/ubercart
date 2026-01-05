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
        'vertical-tabs' => backdrop_get_path('module', 'uc_usps') . '/js/uc_usps.admin.js',
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
  $form['uc_usps_credentials']['newapi']['uc_usps_consumer_key'] = array(
    '#type' => 'textfield',
    '#title' => t('USPS Consumer Key'),
    '#description' => t('Enter your Consumer Key for OAuth2 authorization.'),
    '#default_value' => $config->get('uc_usps_consumer_key'),
    '#size' => 120,
  );
  $consumer_secret = $config->get('uc_usps_consumer_secret');
  $form['uc_usps_credentials']['newapi']['uc_usps_consumer_secret'] = array(
    '#type' => 'password',
    '#title' => t('USPS Consumer Secret'),
    '#description' => empty($consumer_secret) ?
      t('Enter your Consumer Secret for token generation. It will be saved when you save settings but will not be shown after saving for security purposes.') :
      t('You can update your Consumer Secret here if needed, but if the previously saved value is correct, you do not have to enter anything.'),
    '#placeholder' => empty($consumer_secret) ? t('Enter the Consumer Secret') : t('<previous Consumer Secret saved>'),
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
  $access_token = _uc_usps_access_token();
  $token_expiry = new DateTime('@' . state_get('uc_usps_token_expiry'));
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
    '#options' => uc_usps_newapi_dom_env_all_services(),
    '#default_value' => $config->get('uc_usps_newapi_dom_env_services'),
    '#description' => t('Select the USPS services that are available to customers. Be sure to include the services that the Postal Service agrees are available to you.'),
  );

  $form['domestic']['newapi']['uc_usps_newapi_dom_parcel_services'] = array(
    '#type' => 'checkboxes',
    '#title' => t('USPS small package services'),
    '#options' => uc_usps_newapi_dom_parcel_all_services(),
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
    '#options' => uc_usps_newapi_intl_env_all_services(),
    '#default_value' => $config->get('uc_usps_newapi_intl_env_services'),
    '#description' => t('Select the USPS services that are available to customers. Be sure to include the services that the Postal Service agrees are available to you.'),
  );
  $form['international']['newapi']['uc_usps_newapi_intl_parcel_services'] = array(
    '#type' => 'checkboxes',
    '#title' => t('USPS international parcel services'),
    '#options' => uc_usps_newapi_intl_parcel_all_services(),
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
  $options = array(
    'RETAIL' => t('Retail'),
    'COMMERCIAL' => t('Commercial'),
    'CONTRACT' => t('Contract'),
  );
  $form['uc_usps_quote_options']['uc_usps_price_type'] = array(
    '#type' => 'select',
    '#title' => t('Price type'),
    '#description' => t('Select the price type for the price quote from USPS.'),
    '#default_value' => $config->get('uc_usps_price_type'),
    '#options' => $options,
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

  $debug = config_get('uc_quote.settings', 'uc_quote_display_debug');
  $form['uc_usps_debug'] = array(
    '#type' => 'fieldset',
    '#title' => t('Debugging'),
    '#description' => t('These settings select what to display to admins when debugging is turned on. <a href="!url">Debugging is currently @onoff.</a>', array(
      '!url' => url('admin/store/settings/quotes/settings'),
      '@onoff' => $debug ? t('on') : t('off'),
    )),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'usps-settings',
  );
  $form['uc_usps_debug']['uc_usps_debug_request_data'] = array(
    '#type' => 'checkbox',
    '#title' => t('Request data'),
    '#description' => t('Show request data sent to USPS.'),
    '#default_value' => $config->get('uc_usps_debug_request_data'),
  );
  $form['uc_usps_debug']['uc_usps_debug_return_data'] = array(
    '#type' => 'checkbox',
    '#title' => t('Return data'),
    '#description' => t('Show returned data from USPS. Warning: this can be quite large. Checking "Service data" and/or "Include errors" may give sufficient information instead.'),
    '#default_value' => $config->get('uc_usps_debug_return_data'),
  );
  $form['uc_usps_debug']['uc_usps_debug_service_data'] = array(
    '#type' => 'checkbox',
    '#title' => t('Service data'),
    '#description' => t('Show service data (similar to returned data, but more compact).'),
    '#default_value' => $config->get('uc_usps_debug_service_data'),
  );
   $form['uc_usps_debug']['uc_usps_debug_show_errors'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include errors'),
    '#description' => t('Show errors in debug information.'),
    '#default_value' => $config->get('uc_usps_debug_show_errors'),
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
    'uc_usps_consumer_key',
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
    'uc_usps_debug_request_data',
    'uc_usps_debug_return_data',
    'uc_usps_debug_service_data',
    'uc_usps_debug_show_errors',
  );

  foreach ($fields as $key) {
    $value = $form_state['values'][$key];
    if (is_array($value) && isset($form_state['values']['array_filter'])) {
      $value = array_keys(array_filter($value));
    }
    $config->set($key, $value);
  }

  // Update OAuth2 client secret if we entered a value.
  $consumer_secret = $form_state['values']['uc_usps_consumer_secret'];
  if (!empty($consumer_secret)) {
    $config->set('uc_usps_consumer_secret', $consumer_secret);
  }
  // Save all config changes now because _uc_usps_fetch_access_token() will also
  // modify the config file.
  $config->save();
  backdrop_set_message(t('The configuration options have been saved.'));

  // Check token expiration if we're using the new API.
  if ($form_state['values']['uc_usps_authorization_method'] == 'newapi') {
    $token_expiry = state_get('uc_usps_token_expiry');
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


/**
 * Perform a USPS lookup.
 *
 * @see uc_usps_newapi_lookup_form_validate()
 * @see uc_usps_newapi_lookup_form_submit()
 */
function uc_usps_newapi_lookup_form($form, &$form_state) {
  backdrop_set_title(t('USPS Lookup'));
  $last_values = $_SESSION['uc_usps']['usps_lookup_values'] ?? array();
  $form = array();
  $form['#attached']['css'] = array(
    backdrop_get_path('module', 'uc_usps') . '/css/uc_usps.admin.css',
  );
  $form[] = array(
    '#type' => 'help',
    '#markup' => t('Perform a price quote lookup from the USPS using the new API. <a href="!url1", target="_blank">Domestic documentation</a>, <a href="!url2" target="_blank">International documentation</a>.', array(
      '!url1' => 'https://developers.usps.com/domesticpricesv3',
      '!url2' => 'https://developers.usps.com/internationalpricesv3',
    )),
  );
  $form['lookup_type'] = array(
    '#type' => 'radios',
    '#title' => t('Lookup type'),
    '#options' => array(
      'dom_parcel' => t('Domestic parcel'),
      'dom_env' => t('Domestic envelope'),
      'intl_parcel' => t('International parcel'),
      'intl_env' => t('International envelope'),
    ),
    '#default_value' => $last_values['lookup_type'] ?? 'dom_parcel',
  );
  $form['envelope_category'] = array(
    '#type' => 'radios',
    '#title' => t('Envelope category'),
    '#options' => array(
      'LETTERS' => t('Letters'),
      'CARDS' => t('Cards'),
      'FLATS' => t('Flats'),
    ),
    '#default_value' => $last_values['envelope_category'] ?? 'LETTERS',
    '#states' => array(
      'visible' => array(
        array(':input[name="lookup_type"]' => array('value' => 'dom_env')),
        array(':input[name="lookup_type"]' => array('value' => 'intl_env')),
      ),
    ),
  );

  $form['originZIPCode'] = array(
    '#type' => 'textfield',
    '#title' => t('Origin ZIP code'),
    '#default_value' => $last_values['originZIPCode'] ?? '',
    '#required' => TRUE,
  );
  // Destination
  $countries = db_query('
      SELECT country_iso_code_2, country_name
      FROM {uc_countries}
      ')
      ->fetchAllKeyed();
  $form['destinationCountryCode'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#description' => t('Domestic rates apply to these countries: !countries.', array('!countries' => implode(', ', array_values(_uc_usps_domestic_countries_iso2())))),
    '#options' => $countries,
    '#default_value' => $last_values['destinationCountryCode'] ?? 'US',
  );
  // Domestic destination
  $is_domestic_country = array();
  foreach (_uc_usps_domestic_countries_iso2() as $key => $value) {
    $is_domestic_country[] = array(':input[name="destinationCountryCode"]' => array('value' => $key));
  }
  $form['destinationZIPCode'] = array(
    '#type' => 'textfield',
    '#title' => t('Destination ZIP code'),
    '#default_value' => $last_values['destinationZIPCode'] ?? '',
    '#states' => array(
      'visible' => $is_domestic_country,
    ),
  );
  // Foreign destination
  $form['foreignPostalCode'] = array(
    '#type' => 'textfield',
    '#title' => t('Foreign postal code'),
    '#default_value' => $last_values['foreignPostalCode'] ?? '',
    '#states' => array(
      'invisible' => $is_domestic_country,
    ),
  );
  $form['weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Weight'),
    '#description' => t('This is the calculated weight for the package based on user input. The greater of dimWeight (equivalent weight based on the dimensions, calculated by USPS) and weight will be used to calculated the rate. Weight unit of measurement is in pounds.'),
    '#default_value' => $last_values['weight'] ?? '',
  );
  $form['length'] = array(
    '#type' => 'textfield',
    '#title' => t('Length'),
    '#description' => t('This is the package length in inches. The maximum dimension is always length.'),
    '#default_value' => $last_values['length'] ?? '',
  );
  $form['width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#description' => t('This is the package width in inches. The second longest dimension is always width.'),
    '#default_value' => $last_values['width'] ?? '',
    '#states' => array(
      'visible' => array(
        array(':input[name="lookup_type"]' => array('value' => 'dom_parcel')),
        array(':input[name="lookup_type"]' => array('value' => 'intl_parcel')),
      ),
    ),
  );
  $form['height']= array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#description' => t('This is the package height in inches.'),
    '#default_value' => $last_values['height'] ?? '',
  );
  $form['thickness']= array(
    '#type' => 'textfield',
    '#title' => t('Thickness'),
    '#description' => t('This is the thickness in inches.'),
    '#default_value' => $last_values['thickness'] ?? '',
    '#states' => array(
      'visible' => array(
        array(':input[name="lookup_type"]' => array('value' => 'dom_env')),
        array(':input[name="lookup_type"]' => array('value' => 'intl_env')),
      ),
    ),
  );
  $mail_classes_options = array(
    'ALL' => t('All'),
    'PARCEL_SELECT' => t('Parcel Select'),
    'PARCEL_SELECT_LIGHTWEIGHT' => t('Parcel Select Lightweight'),
    'PRIORITY_MAIL_EXPRESS' => t('Priority Mail Express'),
    'PRIORITY_MAIL' => t('Priority Mail'),
    'FIRST_CLASS_PACKAGE_SERVICE' => t('First Class Package Service'),
    'LIBRARY_MAIL' => t('Library Mail'),
    'MEDIA_MAIL' => t('Media Mail'),
    'BOUND_PRINTED_MATTER' => t('Bound Printed Matter'),
    'USPS_CONNECT_LOCAL' => t('USPS Connect Local'),
    'USPS_CONNECT_MAIL' => t('USPS Connect Mail'),
    'USPS_CONNECT_REGIONAL' => t('USPS Connect Regional'),
    'USPS_GROUND_ADVANTAGE' => t('USPS Ground Advantage'),
    'USPS_RETAIL_GROUND' => t('USPS Retail Ground'),
  );
  $form['mailClasses'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Mail classes (domestic)'),
    '#options' => $mail_classes_options,
    '#default_value' => $last_values['mailClasses'] ?? array(),
    '#states' => array(
      'visible' => array(
        ':input[name="lookup_type"]' => array('value' => 'dom_parcel'),
      ),
    ),
  );
  $mail_class_options = array(
    'ALL' => t('All'),
    'FIRST-CLASS_PACKAGE_INTERNATIONAL_SERVICE' => t('First Class Package International Service'),
    'PRIORITY_MAIL_INTERNATIONAL' => t('Priority Mail International'),
    'PRIORITY_MAIL_EXPRESS_INTERNATIONAL' => t('Priority Mail Express International'),
  );
  $form['mailClass'] = array(
    '#type' => 'radios',
    '#title' => t('Mail class (international)'),
    '#options' => $mail_class_options,
    '#default_value' => $last_values['mailClass'] ?? array(),
    '#states' => array(
      'visible' => array(
        ':input[name="lookup_type"]' => array('value' => 'intl_parcel'),
      ),
    ),
  );
  $form['priceType'] = array(
    '#type' => 'radios',
    '#title' => t('Price type'),
    '#options' => array(
      'COMMERCIAL' => t('Commercial'),
      'RETAIL' => t('Retail'),
      'CONTRACT' => t('Contract'),
    ),
    '#default_value' => $last_values['priceType'] ?? array(),
    '#states' => array(
      'visible' => array(
        array(':input[name="lookup_type"]' => array('value' => 'dom_parcel')),
        array(':input[name="lookup_type"]' => array('value' => 'intl_parcel')),
      ),
    ),
  );
  $form['accountType'] = array(
    '#type' => 'select',
    '#title' => t('Account payment type'),
    '#description' => t('The type of payment account linked to a contract rate.'),
    '#options' => array(
      'EPS' => t('EPS'),
      'PERMIT' => t('Permit'),
      'METER' => t('Meter'),
    ),
    '#default_value' => $last_values['accountType'] ?? '',
    '#states' => array(
      'visible' => array(
        ':input[name="priceType"]' => array('value' => 'CONTRACT'),
      ),
    ),
  );
  $form['accountNumber'] = array(
    '#type' => 'textfield',
    '#title' => t('Account number'),
    '#description' => t('The Enterprise Payment Account, Permit number or PC Postage meter number associated with a contract.'),
    '#default_value' => $last_values['accountNumber'] ?? '',
    '#states' => array(
      'visible' => array(
        ':input[name="priceType"]' => array('value' => 'CONTRACT'),
      ),
    ),
  );

  $form['mailingDate'] = array(
    '#type' => 'textfield',
    '#title' => t('Mailing date'),
    '#description' => t('The mailing date should be between 0 and 7 days in the future.'),
    '#default_value' => (new DateTime('@' . (REQUEST_TIME + 86400)))->format('Y-m-d'),
  );
  $form['extraServices'] = array(
    '#type' => 'textfield',
    '#title' => t('Extra services'),
    '#description' => t('Enter any extra services codes separated by commas. See the USPS documentation for allowed values.'),
    '#default_value' => $last_values['extraServices'] ?? '',
  );
  $form['itemValue'] = array(
    '#type' => 'textfield',
    '#title' => t('Item value'),
    '#description' => t('Item value is needed if insurance or registered mail is chosen as an extra service.'),
    '#default_value' => $last_values['itemValue'] ?? 0,
    '#states' => array(
      'visible' => array(
        array(':input[name="lookup_type"]' => array('value' => 'dom_env')),
        array(':input[name="lookup_type"]' => array('value' => 'intl_env')),
      ),
    ),
  );
  $form['hasNonstandardCharacteristics'] = array(
    '#title' => t('Has nonstandard characteristics'),
    '#description' => t('Nonstandard packages include cylindrical tubes and rolls, certain high-density items, cartons containing more than 24 ounces of liquids in one or more glass containers, cartons containing 1 gallon or more of liquid in metal or plastic containers, and items in 201.7.6.2.'),
    '#type' => 'radios',
    '#options' => array(
      '0' => t('No'),
      '1' => t('Yes'),
    ),
    '#default_value' => $last_values['hasNonstandardCharacteristics'] ?? 0,
    '#states' => array(
      'visible' => array(
        array(':input[name="lookup_type"]' => array('value' => 'dom_parcel')),
        array(':input[name="lookup_type"]' => array('value' => 'intl_parcel')),
      ),
    ),
  );
  $form['nonMachinableIndicators'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Non-machinable indicators'),
    '#options' => array(
      'isPolybagged' => t('Is poly-bagged'),
      'hasClosureDevices' => t('Has closure devices'),
      'hasLooseItems' => t('Has loose items'),
      'isRigid' => t('Is rigid'),
      'isSelfMailer' => t('Is self mailer'),
      'isBooklet' => t('Is booklet'),
    ),
    '#default_value' => $last_values['nonMachinableIndicators'] ?? array(),
    '#states' => array(
      'visible' => array(
        array(':input[name="lookup_type"]' => array('value' => 'dom_env')),
        array(':input[name="lookup_type"]' => array('value' => 'intl_env')),
      ),
    ),
  );

  // Selection

  $form['selection'] = array(
    '#type' => 'fieldset',
    '#title' => t('Selection'),
  );
  $form['selection']['destinationEntryFacilityType'] = array(
    '#title' => t('Include all destination entry facility types'),
    '#type' => 'checkbox',
    '#description' => t('Usually the only destination entry facility types of interest are NONE (for domestic) and INTERNATIONAL_SERVICE_CENTER (for international) and others are filtered out. Check this box to include them all.'),
    '#default_value' => $last_values['destinationEntryFacilityType'] ?? 0,
  );
  $form['selection']['additional_mail_classes'] = array(
    '#title' => t('Additional mail classes'),
    '#type' => 'checkboxes',
    '#description' => t('Include returned mail classes that are ordinarily filtered out.'),
    '#options' => array(
      'mail_class_sa' => t('Include mail class SA'),
      'mail_class_marketing' => t('Include mail class USPS_MARKETING_MAIL'),
    ),
    '#default_value' => $last_values['additional_mail_classes'] ?? array(),
  );
  $show_options = array(
    'request' => t('Request'),
    'return_data_krumo' => t('Return data (krumo)'),
    'errors' => t('Errors'),
    'rate_info_basic' => t('Rate information (basic)'),
    'rate_info_full' => t('Rate information (full)'),
    'rate_info_krumo' => t('Rate information (krumo)'),
  );
  if (!module_exists('devel')) {
    unset($show_options['return_data_krumo']);
    unset($show_options['rate_info_krumo']);
  }
  $form['show_options'] = array(
    '#type' => 'value',
    '#value' => $show_options,
  );
  $form['selection']['show'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Show'),
    '#description' => t('Select what types of data to show in the results.'),
    '#options' => $show_options,
    '#default_value' => $last_values['show'] ?? array(),
  );

  // Results

  $form['results'] = array(
    '#type' => 'value',
    '#value' => $form_state['values']['results'] ?? '',
  );
  $form['results_box'] = array(
    '#type' => 'fieldset',
    '#title' => t('Results'),
  );
  $form['results_box']['results_markup'] = array(
    '#type' => 'help',
    '#markup' => $form_state['values']['results'] ?? '',
  );

  $form['#validate'][] = 'uc_usps_newapi_lookup_form_validate';
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#submit' => array('uc_usps_newapi_lookup_form_submit'),
  );
  return $form;
}

/**
 * Validation handler for USPS lookup.
 *
 * @see uc_usps_newapi_lookup_form()
 * @see uc_usps_newapi_lookup_form_submit()
 */
function uc_usps_newapi_lookup_form_validate($form, &$form_state) {
  // If a domestic request, make sure that the country is eligible for domestic
  // rates and vice versa.
  $domestic_request = in_array($form_state['values']['lookup_type'], array('dom_env', 'dom_parcel'));
  $domestic_country = in_array($form_state['values']['destinationCountryCode'], array_keys(_uc_usps_domestic_countries_iso2()));
  if ($domestic_request) {
    if (!$domestic_country) {
      form_set_error('country', t('A domestic rate request can only be used for these countries: !countries.', array('!countries' => implode(', ', array_values(_uc_usps_domestic_countries_iso2())))));
    }
  }
  else {
    if ($domestic_country) {
      form_set_error('country', t('An international rate request cannot be used for these countries: !countries.', array('!countries' => implode(', ', array_values(_uc_usps_domestic_countries_iso2())))));
    }
  }
  // Verify that the mailing date is between 0 and 7 days in the future.
  $mailing_time = strtotime($form_state['values']['mailingDate']);
  $offset = ($mailing_time - REQUEST_TIME) / 86400.0;
  if (!($offset >= 0 && $offset <= 7)) {
    form_set_error('mailingDate', t('The mailing date must be between 0 and 7 days in the future.'));
  }
}

/**
 * Submission handler for USPS lookup'
 *
 * @see uc_usps_newapi_lookup_form()
 * @see uc_usps_newapi_lookup_form_validate()
 */
function uc_usps_newapi_lookup_form_submit($form, &$form_state) {
  $config = config('uc_usps.settings');
  $values = $form_state['values'];
  $_SESSION['uc_usps']['usps_lookup_values'] = $values;
  $connection_address = $config->get('uc_usps_connection_address');

  $extra_services = trim($values['extraServices']);
  if (empty($extra_services)) {
    $extra_services = array();
  }
  else {
    $extra_services = array_map('floatval', array_map('trim', explode(',', $values['extraServices'])));
  }
  $non_machinable_indicators = (object) array_map('boolval', $values['nonMachinableIndicators']);

  $mail_classes = array_keys(array_filter($values['mailClasses']));
  if (in_array('ALL', $mail_classes)) {
    $mail_classes = array('ALL');
  }

  $additional_mail_classes = array_filter($values['additional_mail_classes']);
  $show_options = $values['show_options'];
  $show = array_filter($values['show']);

  switch ($values['lookup_type']) {
    case 'dom_parcel':
      $connection_url = $connection_address . '/prices/v3/total-rates/search';
      $request = array(
        'originZIPCode' => $values['originZIPCode'],
        'destinationZIPCode' => $values['destinationZIPCode'],
        'weight' => (float) $values['weight'],
        'length' => (float) $values['length'],
        'width' => (float) $values['width'],
        'height' => (float) $values['height'],
        'mailClasses' => $mail_classes,
        'priceType' => $values['priceType'],
        'mailingDate' => $values['mailingDate'],
        'hasNonstandardCharacteristics' => (bool) $values['hasNonstandardCharacteristics'],
        'extraServices' => $extra_services,
      );
      break;

    case 'dom_env':
      $connection_url = $connection_address . '/prices/v3/letter-rates/search';
      $request = array(
        'processingCategory' => $values['envelope_category'],
        'weight' => (float) $values['weight'],
        'length' => (float) $values['length'],
        'height' => (float) $values['height'],
        'thickness' => (float) $values['thickness'],
        'mailingDate' => $values['mailingDate'],
        'nonMachinableIndicators' => $non_machinable_indicators,
        'extraServices' => $extra_services,
        'itemValue' => (float) $vlaues['itemValue'],
      );
      break;

    case 'intl_parcel':
      $connection_url = $connection_address . '/international-prices/v3/total-rates/search';
      $request = array(
        'originZIPCode' => $values['originZIPCode'],
        'foreignPostalCode' => $values['foreignPostalCode'],
        'destinationCountryCode' => $values['destinationCountryCode'],
        'weight' => (float) $values['weight'],
        'length' => (float) $values['length'],
        'width' => (float) $values['width'],
        'height' => (float) $values['height'],
        'mailClass' => $values['mailClass'],
        'priceType' => $values['priceType'],
        'mailingDate' => $values['mailingDate'],
        'hasNonstandardCharacteristics' => (bool) $values['hasNonstandardCharacteristics'],
        'extraServices' => $extra_services,
      );
      break;

    case 'intl_env':
      $connection_url = $connection_address . '/international-prices/v3/letter-rates/search';
      $request = array(
        'destinationCountryCode' => $values['destinationCountryCode'],
        'processingCategory' => $values['envelope_category'],
        'weight' => (float) $values['weight'],
        'length' => (float) $values['length'],
        'height' => (float) $values['height'],
        'thickness' => (float) $values['thickness'],
        'mailingDate' => $values['mailingDate'],
        'nonMachinableIndicators' => $non_machinable_indicators,
        'extraServices' => $extra_services,
        'itemValue' => (float) $values['itemValue'],
      );
      break;
  }

  if ($values['priceType'] == 'CONTRACT') {
    $request['accountType'] = $values['accountType'];
    $request['accountNumber'] = $values['accountNumber'];
  }

  $results = '';
  if (isset($show['request'])) {
    $results .= '<strong>' . $show_options['request'] . ':</strong>' . '<pre>' . check_plain(var_export($request, TRUE)) . '</pre>';
  }
  $request_data = backdrop_json_encode($request);
  $access_token = _uc_usps_access_token();
  $usps_result = backdrop_http_request($connection_url, array(
    'method' => 'POST',
    'data' => $request_data,
    'headers' => array(
      'Content-Type' => 'application/json',
      'Authorization' => 'Bearer ' . $access_token,
    ),
  ));
  $data = backdrop_json_decode($usps_result->data);
  if (isset($show['return_data_krumo'])) {
    $results .= '<strong>' . $show_options['return_data_krumo']. ':</strong><br>' . kprint_r($data, TRUE, '$data');
  }
  if (isset($data['error'])) {
    if (isset($show['errors'])) {
      $results .=  '<strong>' . $show_options['errors']. ':</strong><br>' . $data['error']['code'] . ': ' . $data['error']['message'];
    }
  }
  else {
    $rate_info = array();
    if (isset($data['rateOptions'])) {
      // Multiple-rate quote (parcels)
      foreach ($data['rateOptions'] as $rate_option) {
        $rate_row = $rate_option['rates'][0];
        if (!$values['destinationEntryFacilityType']) {
          if (!in_array($rate_row['destinationEntryFacilityType'], array(
            'NONE',
            'INTERNATIONAL_SERVICE_CENTER',
          ))) {
            continue;
          }
        }
        if (!isset($additional_mail_classes['mail_class_sa'])) {
          if ($rate_row['mailClass'] == 'SA') {
            continue;
          }
        }
        if (!isset($additional_mail_classes['mail_class_marketing'])) {
          if ($rate_row['mailClass'] == 'USPS_MAIL_CLASS_MARKETING') {
            continue;
          }
        }
        // Accumulate total price.
        $rate_row['totalPrice'] = $rate_option['totalBasePrice'];
        // Add extra services to the price.
        foreach ($rate_option['extraServices'] as $extra_service) {
          if (in_array((float) $extra_service['extraService'], $extra_services)) {
            $rate_row['totalPrice'] += $extra_service['price'];
          }
        }
        $service_key = _uc_usps_newapi_service_key($rate_row);
        $rate_info[$service_key] = $rate_row;
      }
    }
    else {
      // Single-rate quote (Letters, cards, flats)
      $rate_row = $data['rates'][0];
      $rate_row['totalPrice'] = $data['totalBasePrice'];
      $service_key = _uc_usps_newapi_service_key($rate_row);
      $rate_info[$service_key] = $rate_row;
    }
    if (isset($show['rate_info_basic'])) {
      $results .= '<strong>' . $show_options['rate_info_basic'] . ':</strong>' . _uc_usps_rate_info($rate_info) . '<br>';
    }
    if (isset($show['rate_info_full'])) {
      $results .= '<strong>' . $show_options['rate_info_full'] . ':</strong><pre>' . (var_export($rate_info, TRUE)) . '</pre>';
    }
    if (isset($show['rate_info_krumo'])) {
      $results .= '<strong>' . $show_options['rate_info_krumo'] . ':</strong><br>' . kprint_r($rate_info, TRUE, '$rate_info');
    }
  }
  $form_state['values']['results'] = $results;
  $form_state['rebuild'] = TRUE;
}
