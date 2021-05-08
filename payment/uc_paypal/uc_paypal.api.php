<?php
/**
 * @file
 * Describe hooks provided by the Ubercart PayPal module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Allows alteration of the titles displayed with the PayPal payment method on
 * the checkout page in the payment methods pane.
 *
 * @param string $title_wps
 *   The title to use for Website Payments Standard.
 * @param string $title_ec
 *   The title to use for Express Checkout.
 */
function hook_uc_paypal_titles_alter(&$title_wps, &$title_ec) {
  $title_wps = t('PayPal WPS');
  $title_ec = t('PayPal EC');
}

/**
 * @} End of "addtogroup hooks".
 */
