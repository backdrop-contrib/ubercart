<?php

/**
 * @file
 * Ordel total cost field handler
 */

/**
 * Field handler: display the total cost of an order.
 */
class uc_order_handler_field_order_cost extends uc_order_handler_field_money_amount {

  /**
   *
   */
  function construct() {
    parent::construct();
    $this->additional_fields['order_id'] = 'order_id';
  }

  /**
   *
   */
  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
    // Do nothing else with the query, we'll be retrieving the information directly.
  }

  /**
   *
   */
  function render($values) {
    $this->field_alias = 'order_cost';
    $cost = db_query("SELECT SUM(cost) FROM {uc_order_products} WHERE order_id = :oid", array(':oid' => $values->{$this->aliases['order_id']}))->fetchField();
    $values->{$this->field_alias} = $cost;
    return parent::render($values);
  }

}
