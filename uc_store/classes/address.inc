<?php

/**
 * @file
 * Utility class definition.
 */

/**
 * Defines an object to hold Ubercart mailing address information.
 */
class UcAddress {

  public $first_name = '';
  public $last_name = '';
  public $company = '';
  public $street1 = '';
  public $street2 = '';
  public $city = '';
  public $zone = 0;
  public $postal_code = '';
  public $country = 0;
  public $phone = '';
  public $email = '';

  function __construct() { }

}
