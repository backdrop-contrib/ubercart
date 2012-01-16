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
 * @see http://www.ubercart.org/docs/api/hook_uc_payment_gateway
 *
 * @return
 *   Returns an array of payment gateways, keyed by the gateway ID, and with
 *   the following members:
 *   - "title"
 *     - type: string
 *     - value: The name of the payment gateway displayed to the user. Use t().
 *   - "description"
 *     - type: string
 *     - value: A short description of the payment gateway.
 *   - "settings"
 *     - type: string
 *     - value: The name of a function that returns an array of settings form
 *       elements for the gateway.
 *   - Other keys are payment method IDs, with the value as the name of the
 *     gateway charge function.
 */
function hook_uc_payment_gateway() {
  $gateways['test_gateway'] = array(
    'title' => t('Test gateway'),
    'description' => t('Process credit card payments through the Test Gateway.'),
    'credit' => 'test_gateway_charge',
  );
  return $gateways;
}

/**
 * Alters payment gateways.
 *
 * @param $gateways
 *   Array of payment gateways passed by reference.  Array is structured like
 *   the return value of hook_uc_payment_gateway().
 */
function hook_uc_payment_gateway_alter(&$gateways) {
  // Change the title of the test gateway.
  $gateways['test_gateway']['title'] = t('Altered test gateway title.');
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
  $methods['check'] = array(
    'name' => t('Check'),
    'title' => t('Check or money order'),
    'desc' => t('Pay by mailing a check or money order.'),
    'callback' => 'uc_payment_method_check',
    'weight' => 1,
    'checkout' => TRUE,
  );
  return $methods;
}

/**
 * Alter payment methods.
 *
 * @param $methods
 *   Array of payment methods passed by reference.  Array is structured like
 *   the return value of hook_uc_payment_method().
 */
function hook_uc_payment_method_alter(&$methods) {
  // Change the title of the Check payment method.
  $methods['check']['title'] = t('Cheque');
}

/**
 * @} End of "addtogroup hooks".
 */
