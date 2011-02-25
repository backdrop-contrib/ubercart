<?php

/**
 * @file
 * Views handler: Return payment method name for payment method id
 */

/**
 * Filter by payment method
 */
class uc_order_handler_filter_payment_method extends views_handler_filter_in_operator {

  /**
   *
   */
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Payment Method');
      $this->value_options = array();

      if (module_exists('uc_payment')) {
        $methods = _uc_payment_method_list();
        foreach ($methods as $method) {
          $this->value_options[$method['id']] = $method['title'];
        }
      }
    }
  }

}
