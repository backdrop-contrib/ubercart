<?php

/**
 * @file
 * Hooks provided by the Payment module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Takes action when a payment is entered for an order.
 *
 * @param $order
 *   The order object.
 * @param $method
 *   The name of the payment method used.
 * @param $amount
 *   The value of the payment.
 * @param $account
 *   The user account that entered the order. When the payment is entered
 *   during checkout, this is probably the order's user. Otherwise, it is
 *   likely a store administrator.
 * @param $data
 *   Extra data associated with the transaction.
 * @param $comment
 *   Any comments from the user about the transaction.
 */
function hook_uc_payment_entered($order, $method, $amount, $account, $data, $comment) {
  drupal_set_message(t('User @uid entered a @method payment of @amount for order @order_id.',
    array(
      '@uid' => $account->uid,
      '@method' => $method,
      '@amount' => uc_currency_format($amount),
      '@order_id' => $order->order_id,
    ))
  );
}

/**
 * Registers payment gateway callbacks.
 *
 * @see http://www.ubercart.org/docs/api/hook_payment_gateway
 *
 * @return
 *   Returns an array of payment gateways, which are arrays with the following keys:
 *   - "id"
 *     - type: string
 *     - value: The internal ID of the payment gateway, using a-z, 0-9, and - or
 *         _.
 *   - "title"
 *     - type: string
 *     - value: The name of the payment gateway displayed to the user. Use t().
 *   - "description"
 *     - type: string
 *     - value: A short description of the payment gateway.
 *   - "settings"
 *     - type: string
 *     - value: The name of a function that returns an array of settings form
 *         elements for the gateway.
 */
function hook_uc_payment_gateway() {
  $gateways[] = array(
    'id' => 'test_gateway',
    'title' => t('Test Gateway'),
    'description' => t('Process credit card payments through the Test Gateway.'),
    'credit' => 'test_gateway_charge',
  );
  return $gateways;
}

/**
 * Alter payment gateways.
 *
 * @param $gateways
 *   Payment gateways passed by reference.
 */
function hook_uc_payment_gateway_alter(&$gateways) {
  // Change the title of all gateways.
  foreach ($gateways as &$gateway) {
    // $gateway was passed by reference.
    $gateway['title'] = t('Altered gateway @original', array('@original' => $gateway['title']));
  }
}

/**
 * Registers callbacks for payment methods.
 *
 * Payment methods are different ways to collect payment. By default, Ubercart
 * comes with support for check, credit card, and generic payments. Payment
 * methods show up at checkout or on the order administration screens, and they
 * collect different sorts of information from the user that is used to process
 * or track the payment.
 *
 * @return
 *   An array of payment methods.
 */
function hook_uc_payment_method() {
  $methods[] = array(
    'id' => 'check',
    'name' => t('Check'),
    'title' => t('Check or Money Order'),
    'desc' => t('Pay by mailing a check or money order.'),
    'callback' => 'uc_payment_method_check',
    'weight' => 1,
    'checkout' => TRUE,
  );
  return $methods;
}

/**
 * @} End of "addtogroup hooks".
 */
