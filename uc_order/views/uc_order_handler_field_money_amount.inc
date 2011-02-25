<?php

/**
 * @file
 * Views handler: Product price field.
 */

/**
 * Return a formatted price value to display in the View.
 */
class uc_order_handler_field_money_amount extends uc_product_handler_field_price {

  /**
   *
   */
  function render($values) {
    if ($this->options['format'] == 'numeric') {
      return parent::render($values);
    }

    if ($this->options['format'] == 'uc_price') {
      return uc_currency_format($values->{$this->field_alias});
    }
  }

}
