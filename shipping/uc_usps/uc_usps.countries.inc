<?php

/**
 * @file
 * Contains the map of ISO country codes to USPS Individual Country Listings.
 */

/**
 * Returns the country name formatted according to the USPS requirements.
 *
 * USPS uses the ISO 3166 English short names in most cases.  This function
 * handles the exceptions.
 *
 * @param $code
 *   ISO 3166-1 3-digit numerical country code.
 *
 * @return
 *   Country name string for use by the USPS International Rate API.
 *
 * @see http://pe.usps.gov/text/imm/immctry.htm
 */
function uc_usps_country_map($code = NULL) {
  $countries = array(
    248 => 'Aland Island (Finland)',
  // Heard Island and McDonald Islands
    334 => 'Australia',
    68 => 'Bolivia',
    535 => 'Bonaire (Netherlands Antilles)',
    92 => 'British Virgin Islands',
    166 => 'Cocos Island (Australia)',
    180 => 'Congo, Democratic Republic of the',
    178 => 'Congo, Republic of the',
    531 => 'Curacao (Netherlands Antilles)',
    384 => "Cote d'Ivoire",
    626 => 'East Timor (Indonesia)',
    238 => 'Falkland Islands',
  // French Southern Territories
    260 => 'France',
    268 => 'Georgia, Republic of',
    826 => 'Great Britain and Northern Ireland',
  // British Indian Ocean Territory
    86 => 'Great Britain and Northern Ireland',
  // South Georgia and the
    239 => 'Great Britain and Northern Ireland',
    // South Sandwich Islands
    364 => 'Iran',
  // Palestinian Territory, Occupied
    275 => 'Israel',
    833 => 'Isle of Man (Great Britain and Northern Ireland)',
    498 => 'Moldova',
  // Western Sahara
    732 => 'Morocco',
    408 => "Korea, Democratic People's Republic of (North Korea)",
  // Bouvet Island
    74 => 'Norway',
  // Svalbard and Jan Mayen
    744 => 'Norway',
    410 => 'Korea, Republic of (South Korea)',
    418 => 'Laos',
    492 => 'Monaco (France)',
    104 => 'Myanmar (Burma)',
    612 => 'Pitcairn Island',
    638 => 'Reunion',
    643 => 'Russia',
    688 => 'Serbia, Republic of',
    652 => 'Saint Barthelemy (Guadeloupe)',
    654 => 'Saint Helena',
    534 => 'Saint Maarten (Dutch) (Netherlands Antilles)',
    663 => 'Saint Martin (French) (Guadeloupe)',
    703 => 'Slovak Republic',
    158 => 'Taiwan',
    834 => 'Tanzania',
    792 => 'Turkey',
    804 => 'Ukraine',
    336 => 'Vatican City',
    862 => 'Venezuela',
    876 => 'Wallis and Futuna Islands',
  );

  if ($code) {
    if (isset($countries[$code])) {
      return $countries[$code];
    }
    else {
      return uc_country_get_by_id($code);
    }
  }

  return $countries;
}
