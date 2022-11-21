<?php
/**
 * @file
 * Definition of views_handler_filter_uc_country.
 */

/**
 * Handler for views_handler_filter_uc_country, filter by country.
 *
 * @ingroup views_filter_handlers
 * @see views_handler_filter_uc_zone
 */
class views_handler_filter_uc_country extends views_handler_filter_many_to_one {

  /**
   * Overrides views_handler_filter_many_to_one::get_value_options().
   */
  function get_value_options() {
    $country_data = uc_get_country_data();
    $options = array();
    foreach ($country_data as $info) {
      $options[$info['country_id']] = $info['country_name'];
    }
    $this->value_options = $options;
  }
}
