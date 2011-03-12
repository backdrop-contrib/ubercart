<?php

/**
 * @file
 * Views handler.
 */

/**
 * Filter by country code
 */
class uc_order_handler_filter_country extends views_handler_filter_in_operator {

  /**
   *
   */
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Country');
      $this->value_options = uc_country_option_list();
    }
  }

}
