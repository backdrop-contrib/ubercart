<?php

/**
 * @file
 * Default rules configurations.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function uc_payment_default_rules_configuration() {
  $configs = array();

  // Set the order status to "Payment Received" when a payment is received
  // and the balance is less than or equal to 0.
  $rule = rules_reaction_rule();
  $rule->label = t('Update order status on full payment');
  $rule->active = TRUE;
  $rule->event('uc_payment_entered')
    ->condition('uc_payment_condition_order_balance', array(
      'order:select' => 'order',
      'balance_comparison' => 'less_equal',
    ))
    ->condition(rules_condition('data_is', array(
        'data:select' => 'order:order-status',
        'value' => 'payment_received',
      ))
      ->negate())
    ->action('uc_order_update_status', array(
      'order:select' => 'order',
      'order_status' => 'payment_received',
    ));
  $configs['uc_payment_received'] = $rule;

  // Set the order status to "Completed" when checkout is complete, none
  // of the products are shippable, and the balance is less than or equal to 0.
  $rule = rules_reaction_rule();
  $rule->label = t('Update order status upon checkout completion with full payment');
  $rule->active = TRUE;
  $rule->event('uc_checkout_complete')
    ->condition('uc_payment_condition_order_balance', array(
      'order:select' => 'order',
      'balance_comparison' => 'less_equal',
    ))
    ->condition(rules_condition('uc_order_condition_is_shippable', array(
        'order:select' => 'order',
      ))
      ->negate())
    ->action('uc_order_update_status', array(
      'order:select' => 'order',
      'order_status' => 'completed',
    ));
  $configs['uc_checkout_complete_paid'] = $rule;

  return $configs;
}
