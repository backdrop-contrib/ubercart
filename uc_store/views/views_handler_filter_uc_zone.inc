<?php
/**
 * @file
 * Definition of views_handler_filter_uc_zone.
 */

/**
 * Handler for views_handler_filter_uc_zone, filter by state/province.
 *
 * @ingroup views_filter_handlers
 * @see views_handler_filter_uc_country
 */
class views_handler_filter_uc_zone extends views_handler_filter_many_to_one {

  /**
   * Overrides views_handler_filter_many_to_one::get_value_options().
   */
  function get_value_options() {
    $zone_list = uc_zone_option_list();
    $options = array();
    foreach ($zone_list as $country_list) {
      $options += $country_list;
    }
    $this->value_options = $options;
  }
}
