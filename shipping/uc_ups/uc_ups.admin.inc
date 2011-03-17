<?php

/**
 * @file
 * UPS administration menu items.
 */

/**
 * UPS Online Tool settings.
 *
 * Records UPS account information neccessary to use the service. Allow testing
 * or production mode. Configure which UPS services are quoted to customers.
 *
 * @see uc_admin_settings_validate()
 * @ingroup forms
 */
function uc_ups_admin_settings($form, &$form_state) {
  // Container for credential forms
  $form['uc_ups_credentials'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Credentials'),
    '#description'   => t('Account number and authorization information.'),
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
  );

  $form['uc_ups_credentials']['uc_ups_access_license'] = array(
    '#type' => 'textfield',
    '#title' => t('UPS OnLine Tools XML Access Key'),
    '#default_value' => variable_get('uc_ups_access_license', ''),
    '#required' => TRUE,
  );
  $form['uc_ups_credentials']['uc_ups_shipper_number'] = array(
    '#type' => 'textfield',
    '#title' => t('UPS Shipper #'),
    '#description' => t('The 6-character string identifying your UPS account as a shipper.'),
    '#default_value' => variable_get('uc_ups_shipper_number', ''),
    '#required' => TRUE,
  );
  $form['uc_ups_credentials']['uc_ups_user_id'] = array(
    '#type' => 'textfield',
    '#title' => t('UPS.com user ID'),
    '#default_value' => variable_get('uc_ups_user_id', ''),
    '#required' => TRUE,
  );
  $form['uc_ups_credentials']['uc_ups_password'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#default_value' => variable_get('uc_ups_password', ''),
  );
  $form['uc_ups_credentials']['uc_ups_connection_address'] = array(
    '#type' => 'select',
    '#title' => t('Server mode'),
    '#description' => t('Use the Testing server while developing and configuring your site. Switch to the Production server only after you have demonstartated that transactions on the Testing server are working and you are ready to go live.'),
    '#options' => array('https://wwwcie.ups.com/ups.app/xml/' => t('Testing'),
      'https://www.ups.com/ups.app/xml/' => t('Production'),
    ),
    '#default_value' => variable_get('uc_ups_connection_address', 'https://wwwcie.ups.com/ups.app/xml/'),
  );

  $form['services'] = array(
    '#type' => 'fieldset',
    '#title' => t('Service options'),
    '#description' => t('Set the conditions that will return a UPS quote.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['services']['uc_ups_services'] = array(
    '#type' => 'checkboxes',
    '#title' => t('UPS services'),
    '#default_value' => variable_get('uc_ups_services', _uc_ups_service_list()),
    '#options' => _uc_ups_service_list(),
    '#description' => t('Select the UPS services that are available to customers.'),
  );

  if (module_exists('rules')) {
    $form['services']['service_conditions'] = array(
      '#type' => 'fieldset',
      '#title' => t('UPS service conditions'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $conditions = rules_config_load('get_quote_from_ups');
    if ($conditions) {
      RulesPluginUI::$basePath = 'admin/config/workflow/rules/components';
      $conditions->form($form['services']['service_conditions'], $form_state);
    }
  }

  // Container for quote options
  $form['uc_ups_quote_options'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Quote options'),
    '#description'   => t('Preferences that affect computation of quote.'),
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
  );

  $form['uc_ups_quote_options']['uc_ups_all_in_one'] = array(
    '#type' => 'radios',
    '#title' => t('Product packages'),
    '#default_value' => variable_get('uc_ups_all_in_one', 1),
    '#options' => array(
      0 => t('Each product in its own package'),
      1 => t('All products in one package'),
    ),
    '#description' => t('Indicate whether each product is quoted as shipping separately or all in one package. Orders with one kind of product will still use the package quantity to determine the number of packages needed, however.'),
  );

  // Form to select package types
  $form['uc_ups_quote_options']['uc_ups_pkg_type'] = array(
    '#type' => 'select',
    '#title' => t('Default Package Type'),
    '#default_value' => variable_get('uc_ups_pkg_type', _uc_ups_pkg_types()),
    '#options' => _uc_ups_pkg_types(),
    '#description' => t('Type of packaging to be used.  May be overridden on a per-product basis via the product node edit form.'),
  );
  $form['uc_ups_quote_options']['uc_ups_classification'] = array(
    '#type' => 'select',
    '#title' => t('UPS Customer classification'),
    '#options' => array(
      '01' => t('Wholesale'),
      '03' => t('Occasional'),
      '04' => t('Retail'),
    ),
    '#default_value' => variable_get('uc_ups_classification', '04'),
    '#description' => t('The kind of customer you are to UPS. For daily pickups the default is wholesale; for customer counter pickups the default is retail; for other pickups the default is occasional.'),
  );

  $form['uc_ups_quote_options']['uc_ups_negotiated_rates'] = array(
    '#type' => 'radios',
    '#title' => t('Negotiated rates'),
    '#default_value' => variable_get('uc_ups_negotiated_rates', 0),
    '#options' => array(1 => t('Yes'), 0 => t('No')),
    '#description' => t('Is your UPS account receiving negotiated rates on shipments?'),
  );

  // Form to select pickup type
  $form['uc_ups_quote_options']['uc_ups_pickup_type'] = array(
    '#type' => 'select',
    '#title' => t('Pickup type'),
    '#options' => array(
      '01' => 'Daily Pickup',
      '03' => 'Customer Counter',
      '06' => 'One Time Pickup',
      '07' => 'On Call Air',
      '11' => 'Suggested Retail Rates',
      '19' => 'Letter Center',
      '20' => 'Air Service Center',
    ),
    '#default_value' => variable_get('uc_ups_pickup_type', '01'),
  );

  $form['uc_ups_quote_options']['uc_ups_residential_quotes'] = array(
    '#type' => 'radios',
    '#title' => t('Assume UPS shipping quotes will be delivered to'),
    '#default_value' => variable_get('uc_ups_residential_quotes', 0),
    '#options' => array(
      0 => t('Business locations'),
      1 => t('Residential locations (extra fees)'),
    ),
  );

  $form['uc_ups_quote_options']['uc_ups_unit_system'] = array(
    '#type' => 'select',
    '#title' => t('System of measurement'),
    '#default_value' => variable_get('uc_ups_unit_system', variable_get('uc_length_unit', 'in')),
    '#options' => array(
      'in' => t('British'),
      'cm' => t('Metric'),
    ),
    '#description' => t('Choose the standard system of measurement for your country.'),
  );

  $form['uc_ups_quote_options']['uc_ups_insurance'] = array(
    '#type' => 'checkbox',
    '#title' => t('Package insurance'),
    '#default_value' => variable_get('uc_ups_insurance', TRUE),
    '#description' => t('When enabled, the quotes presented to the customer will include the cost of insurance for the full sales price of all products in the order.'),
  );

  // Container for markup forms
  $form['uc_ups_markups'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Markups'),
    '#description'   => t('Modifiers to the shipping weight and quoted rate.'),
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
  );

  // Form to select type of rate markup
  $form['uc_ups_markups']['uc_ups_rate_markup_type'] = array(
    '#type' => 'select',
    '#title' => t('Rate markup type'),
    '#default_value' => variable_get('uc_ups_rate_markup_type', 'percentage'),
    '#options' => array(
      'percentage' => t('Percentage (%)'),
      'multiplier' => t('Multiplier (×)'),
      'currency' => t('Addition (!currency)', array('!currency' => variable_get('uc_currency_sign', '$'))),
    ),
  );

  // Form to select rate markup amount
  $form['uc_ups_markups']['uc_ups_rate_markup'] = array(
    '#type' => 'textfield',
    '#title' => t('Shipping rate markup'),
    '#default_value' => variable_get('uc_ups_rate_markup', '0'),
    '#description' => t('Markup shipping rate quote by currency amount, percentage, or multiplier.'),
  );

  // Form to select type of weight markup
  $form['uc_ups_markups']['uc_ups_weight_markup_type'] = array(
    '#type'          => 'select',
    '#title'         => t('Weight markup type'),
    '#default_value' => variable_get('uc_ups_weight_markup_type', 'percentage'),
    '#options'       => array(
      'percentage' => t('Percentage (%)'),
      'multiplier' => t('Multiplier (×)'),
      'mass'       => t('Addition (!mass)', array('!mass' => '#')),
    ),
    '#disabled' => TRUE,
  );

  // Form to select weight markup amount
  $form['uc_ups_markups']['uc_ups_weight_markup'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Shipping weight markup'),
    '#default_value' => variable_get('uc_ups_weight_markup', '0'),
    '#description'   => t('Markup UPS shipping weight on a per-package basis before quote, by weight amount, percentage, or multiplier.'),
    '#disabled' => TRUE,
  );

  // Taken from system_settings_form(). Only, don't use its submit handler.
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  if (!empty($_POST) && form_get_errors()) {
    drupal_set_message(t('The settings have not been saved because of the errors.'), 'error');
  }
  if (!isset($form['#theme'])) {
    $form['#theme'] = 'system_settings_form';
  }

  return $form;
}

/**
 * Validation handler for uc_ups_admin_settings.
 *
 * Requires password only if it hasn't been set.
 *
 * @see uc_ups_admin_settings()
 * @see uc_ups_admin_settings_submit()
 */
function uc_ups_admin_settings_validate($form, &$form_state) {
  $old_password = variable_get('uc_ups_password', '');
  if (!$form_state['values']['uc_ups_password']) {
    if ($old_password) {
      form_set_value($form['uc_ups_credentials']['uc_ups_password'], $old_password, $form_state);
    }
    else {
      form_set_error('uc_ups_password', t('Password field is required.'));
    }
  }

  if (!is_numeric($form_state['values']['uc_ups_rate_markup'])) {
    form_set_error('uc_ups_rate_markup', t('Rate markup must be a numeric value.'));
  }
  if (!is_numeric($form_state['values']['uc_ups_weight_markup'])) {
    form_set_error('uc_ups_weight_markup', t('Weight markup must be a numeric value.'));
  }
}

/**
 * Submit handler for uc_ups_admin_settings().
 *
 * Emulates system_settings_form_submit(), but only on a subset of the
 * form values.
 *
 * @see uc_ups_admin_settings()
 * @see uc_ups_admin_settings_validate()
 */
function uc_ups_admin_settings_submit($form, &$form_state) {
  $fields = array(
    'uc_ups_access_license',
    'uc_ups_shipper_number',
    'uc_ups_user_id',
    'uc_ups_password',
    'uc_ups_connection_address',
    'uc_ups_services',
    'uc_ups_pickup_type',
    'uc_ups_pkg_type',
    'uc_ups_classification',
    'uc_ups_negotiated_rates',
    'uc_ups_residential_quotes',
    'uc_ups_rate_markup_type',
    'uc_ups_rate_markup',
    'uc_ups_weight_markup_type',
    'uc_ups_weight_markup',
    'uc_ups_all_in_one',
    'uc_ups_unit_system',
    'uc_ups_insurance',
  );

  foreach ($fields as $key) {
    $value = $form_state['values'][$key];

    if (is_array($value) && isset($form_state['values']['array_filter'])) {
      $value = array_keys(array_filter($value));
    }
    variable_set($key, $value);
  }

  drupal_set_message(t('The configuration options have been saved.'));

  cache_clear_all();
  drupal_theme_rebuild();
}

/**
 * Last chance for user to review shipment.
 *
 * @see uc_ups_confirm_shipment_submit()
 * @see theme_uc_ups_confirm_shipment()
 * @ingroup forms
 */
function uc_ups_confirm_shipment($form, &$form_state, $order) {
  $form['digest'] = array('#type' => 'hidden', '#value' => $_SESSION['ups']['digest']);
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Request Pickup'));

  return $form;
}

/**
 * Displays final shipment information for review.
 *
 * @see uc_ups_confirm_shipment()
 * @ingroup themeable
 */
function theme_uc_ups_confirm_shipment($variables) {
  $form = $variables['form'];

  $output = '<div class="shipping-address"><b>' . t('Ship from:') . '</b><br />';
  $output .= uc_address_format(
    check_plain($_SESSION['ups']['origin']->first_name),
    check_plain($_SESSION['ups']['origin']->last_name),
    check_plain($_SESSION['ups']['origin']->company),
    check_plain($_SESSION['ups']['origin']->street1),
    check_plain($_SESSION['ups']['origin']->street2),
    check_plain($_SESSION['ups']['origin']->city),
    check_plain($_SESSION['ups']['origin']->zone),
    check_plain($_SESSION['ups']['origin']->postal_code),
    check_plain($_SESSION['ups']['origin']->country)
  );
  $output .= '<br />' . check_plain($_SESSION['ups']['origin']->email);
  $output .= '</div>';

  $output .= '<div class="shipping-address"><b>' . t('Ship to:') . '</b><br />';
  $output .= uc_address_format(
    check_plain($_SESSION['ups']['destination']->first_name),
    check_plain($_SESSION['ups']['destination']->last_name),
    check_plain($_SESSION['ups']['destination']->company),
    check_plain($_SESSION['ups']['destination']->street1),
    check_plain($_SESSION['ups']['destination']->street2),
    check_plain($_SESSION['ups']['destination']->city),
    check_plain($_SESSION['ups']['destination']->zone),
    check_plain($_SESSION['ups']['destination']->postal_code),
    check_plain($_SESSION['ups']['destination']->country)
  );
  $output .= '<br />' . check_plain($_SESSION['ups']['destination']->email);
  $output .= '</div>';
  $output .= '<div class="shipment-data">';
  $method = uc_ups_uc_shipping_method();
  $output .= '<b>' . $method['ups']['quote']['accessorials'][$_SESSION['ups']['service']] . '</b><br />';
  $output .= '<i>' . check_plain($_SESSION['ups']['rate']['type']) . '</i>: ' . theme('uc_price', array('price' => $_SESSION['ups']['rate']['amount'])) . ' (' . check_plain($_SESSION['ups']['rate']['currency']) . ')<br />';
  $ship_date = $_SESSION['ups']['ship_date'];
  $output .= 'Ship date: ' . format_date(gmmktime(12, 0, 0, $ship_date['month'], $ship_date['day'], $ship_date['year']), 'uc_store');
  $exp_delivery = $_SESSION['ups']['expected_delivery'];
  $output .= '<br />Expected delivery: ' . format_date(gmmktime(12, 0, 0, $exp_delivery['month'], $exp_delivery['day'], $exp_delivery['year']), 'uc_store');
  $output .= "</div>\n<br style=\"clear: both;\" />";
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Generates label and schedules pickup of the shipment.
 *
 * @see uc_ups_confirm_shipment()
 */
function uc_ups_confirm_shipment_submit($form, &$form_state) {
  // Request pickup using parameters in form.
  $order_id = $_SESSION['ups']['order_id'];
  $packages = array_keys($_SESSION['ups']['packages']);
  $request = uc_ups_request_pickup($form_state['values']['digest'], $order_id, $packages);
  $result = drupal_http_request(variable_get('uc_ups_connection_address', 'https://wwwcie.ups.com/ups.app/xml/') . 'ShipAccept', array(
    'method' => 'POST',
    'data' => $request,
  ));
  $response = new SimpleXMLElement($result->data);
  $code = (string)$response->Response->ResponseStatusCode;
  if ($code == 0) { // failed request
    $error = $response->Response->Error;
    $error_severity = (string)$error->ErrorSeverity;
    $error_code = (string)$error->ErrorCode;
    $error_description = (string)$error->ErrorDescription;
    drupal_set_message(t('(@severity error @code) @description', array('@severity' => $error_severity, '@code' => $error_code, '@description' => $error_description)), 'error');
    if ($error_severity == 'HardError') {
      $form_state['redirect'] = 'admin/store/orders/' . $order_id . '/shipments/ups/' . implode('/', $packages);
      return;
    }
  }

  $shipment = new stdClass();
  $shipment->order_id = $order_id;
  $shipment->origin = clone $_SESSION['ups']['origin'];
  $shipment->destination = clone $_SESSION['ups']['destination'];
  $shipment->packages = $_SESSION['ups']['packages'];
  $shipment->shipping_method = 'ups';
  $shipment->accessorials = $_SESSION['ups']['service'];
  $shipment->carrier = t('UPS');
  // if NegotiatedRates exist, quote based on those, otherwise, use TotalCharges
  if (isset($response->ShipmentResults->ShipmentCharges)) {
    $charge = $response->ShipmentResults->ShipmentCharges->TotalCharges;
    if (isset($response->ShipmentResults->NegotiatedRates)) {
      $charge = $response->ShipmentResults->NegotiatedRates->NetSummaryCharges->GrandTotal;
    }
  }
  $cost = (string)$charge->MonetaryValue;
  $shipment->cost = $cost;
  $shipment->tracking_number = (string)$response->ShipmentResults->ShipmentIdentificationNumber;
  $ship_date = $_SESSION['ups']['ship_date'];
  $shipment->ship_date = gmmktime(12, 0, 0, $ship_date['month'], $ship_date['day'], $ship_date['year']);
  $exp_delivery = $_SESSION['ups']['expected_delivery'];
  $shipment->expected_delivery = gmmktime(12, 0, 0, $exp_delivery['month'], $exp_delivery['day'], $exp_delivery['year']);

  foreach ($response->ShipmentResults->PackageResults as $package_results) {
    $package =& current($shipment->packages);
    $package->tracking_number = (string)$package_results->TrackingNumber;
    $label_image = (string)$package_results->LabelImage->GraphicImage;
    if (file_preprare_directory('public://ups_labels', FILE_CREATE_DIRECTORY)) {
      $label_path = 'public://ups_labels/label' . $package->tracking_number . '.gif';
      if ($label_file = file_save_data(base64_decode($label_image), $label_path, FILE_EXISTS_REPLACE)) {
        file_usage_add($label_file, 'uc_shipping', 'package', $package->package_id);
        $package->label_image = $label_file;
      }
      else {
        drupal_set_message(t('Could not open a file to save the label image.'), 'error');
      }
    }
    else {
      drupal_set_message(t('Could not find or create the directory "ups_labels" in the file system path.'), 'error');
    }
    unset($package);
    next($shipment->packages);
  }

  uc_shipping_shipment_save($shipment);

  unset($_SESSION['ups']);
  $form_state['redirect'] = 'admin/store/orders/' . $order_id . '/shipments';
}

/**
 * Constructs an XML label and pickup request.
 *
 * @param $digest
 *   Base-64 encoded shipment request.
 * @param $order_id
 *   The order id of the shipment.
 * @param $packages
 *   An array of package ids to be shipped.
 *
 * @return
 *   ShipmentAcceptRequest XML document to send to UPS.
 */
function uc_ups_request_pickup($digest, $order_id = 0, $packages = array()) {
  $packages = (array)$packages;

  $schema = uc_ups_access_request();
  $schema .= "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<ShipmentAcceptRequest>
  <Request>
    <RequestAction>ShipAccept</RequestAction>";
    if ($order_id || count($packages)) {
      $schema .= "\n<TransactionReference>
      <CustomerContext>";
      if ($order_id) {
        $schema .= "<OrderId>" . $order_id . "</OrderId>\n";
      }
      foreach ($packages as $pkg_id) {
        $schema .= "<PackageId>" . $pkg_id . "</PackageId>\n";
      }
      $schema .= "</CustomerContext>\n</TransactionReference>\n";
    }
  $schema .= "  </Request>
  <ShipmentDigest>" . $digest . "</ShipmentDigest>
</ShipmentAcceptRequest>";

  return $schema;
}

/**
 * Displays the shipping label for printing.
 *
 * Each argument is a component of the file path to the image.
 *
 * @ingroup themeable
 */
function theme_uc_ups_label_image() {
  $args = explode('/', $_GET['q'], 7);
  if (count($args) != 7) {
    return MENU_NOT_FOUND;
  }

  $image_path = file_stream_wrapper_uri_normalize($args[6]);

  $output = <<<EOLABEL
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 3.2//EN">
<html>
<head>
<title>View/Print Label</title>
<style>
  .small-text {font-size: 80%;}
  .large-text {font-size: 115%;}
</style>
</head>
<body bgcolor="#FFFFFF">
<table border="0" cellpadding="0" cellspacing="0" width="600"><tr>
<td height="410" align="left" valign="top">
<b class="large-text">View/Print Label</b>
&nbsp;<br />
<ol class="small-text"> <li><b>Print the label:</b> &nbsp;
Select Print from the File menu in this browser window to print the label below.<br /><br /><li><b>
Fold the printed label at the dotted line.</b> &nbsp;
Place the label in a UPS Shipping Pouch. If you do not have a pouch, affix the folded label using clear plastic shipping tape over the entire label.<br /><br /><li><b>GETTING YOUR SHIPMENT TO UPS<br />
Customers without a Daily Pickup</b><ul><li>Ground, 3 Day Select, and Standard to Canada shipments must be dropped off at an authorized UPS location, or handed to a UPS driver. Pickup service is not available for these services. To find the nearest drop-off location, select the Drop-off icon from the UPS tool bar.<li>
Air shipments (including Worldwide Express and Expedited) can be picked up or dropped off. To schedule a pickup, or to find a drop-off location, select the Pickup or Drop-off icon from the UPS tool bar.  </ul> <br />
<b>Customers with a Daily Pickup</b><ul><li>
Your driver will pickup your shipment(s) as usual. </ul>
</ol></td></tr></table><table border="0" cellpadding="0" cellspacing="0" width="600">
<tr>
<td class="small-text" align="left" valign="top">
&nbsp;&nbsp;&nbsp;
FOLD HERE</td>
</tr>
<tr>
<td align="left" valign="top"><hr />
</td>
</tr>
</table>

<table>
<tr>
<td height="10">&nbsp;
</td>
</tr>
</table>

<table border="0" cellpadding="0" cellspacing="0" width="650" ><tr>
<td align="left" valign="top">
<img src="$image_path" height="392" width="672">
</td>
</tr></table>
</body>
</html>
EOLABEL;

  print $output;
  exit();
}
