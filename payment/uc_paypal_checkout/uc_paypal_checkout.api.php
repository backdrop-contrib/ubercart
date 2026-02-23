<?php

/**
 * @file
 * Documents hooks provided by the Ubercart PayPal Checkout module.
 */

/**
 * Allows modules to alter the request body before the create order API call is
 * made to PayPal.
 *
 * @param array $request_body
 *   The request body.
 * @param UcOrder $order
 *   The order.
 */
function hook_uc_paypal_checkout_create_order_request_alter(&$request_body, $order) {
 // No example.
}
