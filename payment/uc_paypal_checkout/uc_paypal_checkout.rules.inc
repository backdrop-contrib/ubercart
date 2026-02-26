<?php
/**
 * @file
 * Rules hooks for uc_paypal_checkout.module.
 */

/**
 * Implements hook_rules_event_info().
 */
function uc_paypal_checkout_rules_event_info() {
  $events = array(
    'uc_paypal_checkout_event_voided' => array(
      'label' => t('Paypal Checkout authorization was voided'),
      'group' => t('PayPal Checkout'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
    'uc_paypal_checkout_event_refunded' => array(
      'label' => t('Paypal Checkout payment refunded'),
      'group' => t('PayPal Checkout'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
    'uc_paypal_checkout_event_completed' => array(
      'label' => t('Paypal Checkout completed payment'),
      'group' => t('PayPal Checkout'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
    'uc_paypal_checkout_event_pending' => array(
      'label' => t('Paypal Checkout payment is pending'),
      'group' => t('PayPal Checkout'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
    'uc_paypal_checkout_event_verification_fail' => array(
      'label' => t('Paypal Checkout transaction verification failed'),
      'group' => t('PayPal Checkout'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
  );

  return $events;
}
