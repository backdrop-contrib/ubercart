<?php
/**
 * @file
 * Development functions for USPS shipping quote module.
 */

/**
 * Function for getting sample new API quotes and optionally storing them in a
 * CSV file. Useful for seeing USPS responses for various inputs when developing
 * or debugging; we also use this to get current lists of provided services.
 *
 * @param string $region
 *   one of 'dom' (domestic) or 'intl' (international).
 * @param string $price_type
 *   one of 'RETAIL', 'COMMERCIAL' or 'CONTRACT'.
 * @param string $mail_class
 *   one of 'ALL', 'LETTERS', 'CARDS', 'FLATS'.
 * @param array $extras
 *   extra services (integer codes, see USPS API).
 * @param string $csv_dir
 *   directory to export a CSV version of the returned array to. If empty, there
 *   is no export.
 *
 * @return array
 *   an array of all rate information returned by the lookup call but filtered
 *   to only the destinationEntryFacilityType of 'NONE' (for domestic) or
 *   'INTERNATIONAL_SERVICE_CENTER' (for international).
 *
 * See domestic APIs: https://developers.usps.com/domesticpricesv3
 * See international APIs: https://developers.usps.com/internationalpricesv3
 */
function uc_usps_newapi_sample_quote($region, $price_type, $mail_class, $extras = array(), $csv_dir = '') {
  $config = config('uc_usps.settings');
  $access_token = uc_usps_access_token();
  $mailing_date = (new DateTime('@' . (REQUEST_TIME)))->format('Y-m-d');

  // Simple validation of all arguments.
  $valid_args = array(
    array('dom', 'intl'),
    array('RETAIL', 'COMMERCIAL', 'CONTRACT'),
    array('ALL', 'LETTERS', 'CARDS', 'FLATS'),
  );
  $args = array($region, $price_type, $mail_class);
  for ($idx = 0; $idx < 3; $idx++) {
    if (!in_array($args[$idx], $valid_args[$idx])) {
      backdrop_set_message(t('%arg is not in [@valid_args].', array(
        '%arg' => args[$idx],
        '@valid_args' => implode(', ', $valid_args[$idx]),
      )), 'error');
      return array();
    }
  }

  if ($region == 'dom') { // domestic
    switch ($mail_class) {
      case 'ALL':
        $rate_search = '/total-rates/search';
        $request = array(
          'originZIPCode' => '22407',
          'destinationZIPCode' => '63118',
          'weight' => 0.5,
          'length' => 5,
          'width' => 3,
          'height' => 1,
          'mailClasses' => [ 'ALL', ],
          'priceType' => $price_type,
          'mailingDate' => $mailing_date,
        );
        if (!empty($extras)) {
          $request['extraServices'] = $extras;
        }
        break;

      case 'LETTERS':
        $rate_search = '/letter-rates/search';
        $request = array(
          'weight' => 0.01,
          'length' => 9.0, // 5.0 <= L <= 11.5
          'height' => 5.0, // 2.5 <= H <= 6.125
          'thickness' => 0.10, // 0.007 <= T <= 0.25
          'processingCategory' => 'LETTERS',
          'mailingDate' => $mailing_date,
        );
        if (!empty($extra)) {
          $request['nonMachinableIndicators'] = $extras;
        }
        break;

      case 'CARDS':
        $rate_search = '/letter-rates/search';
        $request = array(
          'weight' => 0.01,
          'length' => 5.5, // 5.0 <= L <= 6.0
          'height' => 4.0, // 3.5 <= H <= 4.25
          'thickness' => 0.010, // 0.007 < T <= 0.016
          'processingCategory' => 'CARDS',
          'mailingDate' => $mailing_date,
        );
        if (!empty($extra)) {
          $request['nonMachinableIndicators'] = $extras;
        }
        break;

      case 'FLATS':
        $rate_search = '/letter-rates/search';
        $request = array(
          'weight' => 0.01,
          // At least one of L >= 11.5 or H >= 6.125 or T >= 0.25
          'length' => 14.0, // L <= 15.0
          'height' => 10.0, // H <= 12.0
          'thickness' => 0.50, // T <= 0.75
          'processingCategory' => 'FLATS',
          'mailingDate' => $mailing_date,
        );
        if (!empty($extra)) {
          $request['nonMachinableIndicators'] = $extras;
        }
        break;

      default:
        return array();
    }
  }
  else { // internatioal
    switch ($mail_class) {
      case 'ALL':
        $rate_search = '/total-rates/search';
        $request = array(
          'originZIPCode' => '22407',
          'foreignPostalCode' => 'WC2H 0EB',
          'destinationCountryCode' => 'GB',
          'weight' => 0.5,
          'length' => 6,
          'width' => 4,
          'height' => 2,
          'mailClass' => 'ALL',
          'priceType' => $price_type,
          'mailingDate' => $mailing_date,
        );
        if (!empty($extras)) {
          $request['extraServices'] = $extras;
        }
        break;

      case 'LETTERS':
        $rate_search = '/letter-rates/search';
        $request = array(
          'destinationCountryCode' => 'GB',
          'weight' => 0.01,
          'length' => 9.0, // 5.0 <= L <= 11.5
          'height' => 5.0, // 2.5 <= H <= 6.125
          'thickness' => 0.10, // 0.007 <= T <= 0.25
          'processingCategory' => 'LETTERS',
          'mailingDate' => $mailing_date,
        );
        if (!empty($extra)) {
          $request['nonMachinableIndicators'] = $extras;
        }
        break;

      case 'CARDS':
        $rate_search = '/letter-rates/search';
        $request = array(
          'destinationCountryCode' => 'GB',
          'weight' => 0.01,
          'length' => 5.5, // 5.0 <= L <= 6.0
          'height' => 4.0, // 3.5 <= H <= 4.25
          'thickness' => 0.010, // 0.007 < T <= 0.016
          'processingCategory' => 'CARDS',
          'mailingDate' => $mailing_date,
        );
        if (!empty($extra)) {
          $request['nonMachinableIndicators'] = $extras;
        }
        break;

      case 'FLATS':
        $rate_search = '/letter-rates/search';
        $request = array(
          'destinationCountryCode' => 'GB',
          'weight' => 0.01,
          // At least one of L >= 11.5 or H >= 6.125 or T >= 0.25
          'length' => 14.0, // L <= 15.0
          'height' => 10.0, // H <= 12.0
          'thickness' => 0.50, // T <= 0.75
          'processingCategory' => 'FLATS',
          'mailingDate' => $mailing_date,
        );
        if (!empty($extra)) {
          $request['nonMachinableIndicators'] = $extras;
        }
        break;

      default:
        return array();
    }
  }

  if ($region == 'dom') {
    $connection_url = $config->get('uc_usps_connection_address') . '/prices/v3' . $rate_search;
  }
  else {
    $connection_url = $config->get('uc_usps_connection_address') . '/international-prices/v3' . $rate_search;
  }
  $request_data = backdrop_json_encode($request);
  $result = backdrop_http_request($connection_url, array(
    'method' => 'POST',
    'data' => $request_data,
    'headers' => array(
      'Content-Type' => 'application/json',
      'Authorization' => 'Bearer ' . $access_token,
    ),
  ));

  $data = backdrop_json_decode($result->data);
  if (isset($data['error'])) {
    backdrop_set_message(t('Error @code: @message', array(
      '@code' => $data['error']['code'],
      '@message' => $data['error']['message'],
    )), 'error');
  }
  $rate_info = array();
  // These are the only acceptable destination facilities (for domestic and
  // international parcel mail, respectively).
  $destination_facilities = array('NONE', 'INTERNATIONAL_SERVICE_CENTER');
  // These mail classes are returned for parcel mail but we don't support them.
  $forbidden_mail_classes = array('SA', 'USPS_MARKETING_MAIL');
  switch ($mail_class) {
    case 'ALL':
      if (isset($data['rateOptions'])) {
        foreach ($data['rateOptions'] as $rate_option) {
          $rates = $rate_option['rates'];
          $rate_row = $rate_option['rates'][0] + array(
            'totalBasePrice' => $rate_option['totalBasePrice'],
          );

          // Don't include special destination facilities for domestic mail.
          // Don't include mailClass of SA, or USPS_MARKETING_MAIL.
          if (in_array($rate_row['destinationEntryFacilityType'], $destination_facilities) && !in_array($rate_row['mailClass'], $forbidden_mail_classes)) {
            $rate_info[] = $rate_row;
          }
        }
      }
      break;

    default: // LETTERS, CARDS, FLAT
      $rate_row = $data['rates'][0] + array(
        'totalBasePrice' => $data['totalBasePrice'],
      );
      $rate_info[] = $rate_row;
  }

  // Augment each row with its service key.
  foreach ($rate_info as $idx => $info) {
    $rate_info[$idx]['service_key'] = _uc_usps_newapi_service_key($info);
  }

  if (!empty($csv_dir) && !empty($rate_info)) {
    // Convert to CSV and export to file with header.
    module_load_include('inc', 'uc_reports', 'uc_reports.admin');
    // Add a header consisting of the element keys.
    $rate_info = array_merge(array(array_keys($rate_info[0])), $rate_info);
    $csv_data = uc_reports_store_csv('rate_info', $rate_info);
    $csv_file = "{$csv_dir}/rate_info_{$region}_{$price_type}_{$mail_class}.csv";
    $result = file_put_contents($csv_file, $csv_data['csv']);
  }
  return $rate_info;
}

/**
 * Write CSV files of all possible combinations of base rate settings, but not
 * including any extras.
 */
function uc_usps_newapi_sample_quotes_all($csv_dir) {
  if (empty($csv_dir)) {
    return;
  }
  foreach (array('dom', 'intl') as $region) {
    foreach (array('RETAIL', 'COMMERCIAL', 'CONTRACT') as $price_type) {
      foreach (array('ALL', 'LETTERS', 'CARDS', 'FLATS') as $mail_class) {
        uc_usps_newapi_sample_quote($region, $price_type, $mail_class, [], $csv_dir);
      }
    }
  }
}

/**
 * Set accessorials (services) for domestic/international and envelope/parcel
 * as config arrays and optionally create a PHP file that sets the config
 * values.
 *
 * This is a utility routine used to set up the initial array stored in default
 * config, but it might be needed in the future if USPS changes what services
 * they offer within the new API.
 *
 * @param string $config_dir
 *   If non-empty, the directory to write the PHP config command file to set the
 *   config values, used in the hook_update_N implementation.
 */
function uc_usps_newapi_fetch_services($config_dir = '') {
  $config = config('uc_usps.settings');
  $write_config = !empty($config_dir);
  $text = '';

  // Domestic envelope. Include letters, but also include some parcel services
  // because legacy included ordinary "Priority Mail" and "Express Mail".
  $services = array();
  $rate_info = array_merge(
    uc_usps_newapi_sample_quote('dom', 'COMMERCIAL', 'LETTERS'),
    uc_usps_newapi_sample_quote('dom', 'COMMERCIAL', 'CARDS'),
    uc_usps_newapi_sample_quote('dom', 'COMMERCIAL', 'FLATS'),
    uc_usps_newapi_sample_quote('dom', 'COMMERCIAL', 'ALL')
  );
  $forbidden_services = array(
    'Flat Rate Box',
    'Cubic',
    'Bound Printed Matter',
    'Library',
    'Media',
  );
  foreach ($rate_info as $info) {
    foreach ($forbidden_services as $bad_word) {
      if (strpos($info['description'], $bad_word) !== FALSE) {
        continue 2;
      }
    }
    $services[$info['service_key']] = $info['description'];
  }
  $config->set('uc_usps_newapi_dom_env_all_services', $services);
  if ($write_config) {
    $text .= uc_usps_newapi_create_config('uc_usps_newapi_dom_env_all_services', $services);
  }

  // Domestic parcel. No cards or flat-rate envelopes to match legacy. But we
  // do include first-class letters to match legacy.
  $services = array();
  $rate_info = array_merge(
    uc_usps_newapi_sample_quote('dom', 'COMMERCIAL', 'LETTERS'),
    uc_usps_newapi_sample_quote('dom', 'COMMERCIAL', 'FLATS'),
    uc_usps_newapi_sample_quote('dom', 'COMMERCIAL', 'ALL')
  );
  $forbidden_services = array(
    'Flat Rate Envelope',
  );
  foreach ($rate_info as $info) {
    foreach ($forbidden_services as $bad_word) {
      if (strpos($info['description'], $bad_word) !== FALSE) {
        continue 2;
      }
    }
    $services[$info['service_key']] = $info['description'];
  }
  $config->set('uc_usps_newapi_dom_parcel_all_services', $services);
  if ($write_config) {
    $text .= uc_usps_newapi_create_config('uc_usps_newapi_dom_parcel_all_services', $services);
  }

  // International envelope. Include letters, but also include parcel services
  // because legacy included ordinary "Priority Mail" and "Express Mail".
  $services = array();
  $rate_info = array_merge(
    uc_usps_newapi_sample_quote('intl', 'COMMERCIAL', 'LETTERS'),
    uc_usps_newapi_sample_quote('intl', 'COMMERCIAL', 'CARDS'),
    uc_usps_newapi_sample_quote('intl', 'COMMERCIAL', 'FLATS'),
    uc_usps_newapi_sample_quote('intl', 'COMMERCIAL', 'ALL')
  );
  $forbidden_services = array(
    'Flat Rate Box',
  );
  foreach ($rate_info as $info) {
    foreach ($forbidden_services as $bad_word) {
      if (strpos($info['description'], $bad_word) !== FALSE) {
        continue 2;
      }
    }
    $services[$info['service_key']] = $info['description'];
  }
  $config->set('uc_usps_newapi_intl_env_all_services', $services);
  if ($write_config) {
    $text .= uc_usps_newapi_create_config('uc_usps_newapi_intl_env_all_services', $services);
  }

  // International parcel. No cards or envelopes to match legacy.
  $services = array();
  $rate_info = array_merge(
    uc_usps_newapi_sample_quote('intl', 'COMMERCIAL', 'ALL')
  );
  $forbidden_services = array(
    'Flat Rate Envelope',
  );
  foreach ($rate_info as $info) {
    foreach ($forbidden_services as $bad_word) {
      if (strpos($info['description'], $bad_word) !== FALSE) {
        continue 2;
      }
    }
    $services[$info['service_key']] = $info['description'];
  }
  $config->set('uc_usps_newapi_intl_parcel_all_services', $services);
  if ($write_config) {
    $text .= uc_usps_newapi_create_config('uc_usps_newapi_intl_parcel_all_services', $services);
  }

  $config->save();
  if ($write_config) {
    $php_file = "{$config_dir}/uc_usps.services.php";
    $result = file_put_contents($php_file, "<?php\n" . $text);
  }
}

/**
 * Create a string that sets a config value from an array of services. This is
 * used to generate the PHP code in a hook_update_N() implementation.
 */
function uc_usps_newapi_create_config($value, $services) {
  $text = "  \$config->set('{$value}', array(\n";
  foreach ($services as $key => $value) {
    $text .= "    '" . $key ."' => '" . $value . "',\n";
  }
  $text .= "  ));\n";
  return $text;
}
