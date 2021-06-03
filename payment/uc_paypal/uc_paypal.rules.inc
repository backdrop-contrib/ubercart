<?php
/**
 * @file
 * Rules hooks for uc_paypal.module.
 */

/**
 * Implements hook_rules_event_info().
 */
function uc_paypal_rules_event_info() {
  $events = array(
    'uc_paypal_event_cancelled_reversal' => array(
      'label' => t('Paypal cancelled payment reversal'),
      'group' => t('PayPal'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
    'uc_paypal_event_completed' => array(
      'label' => t('Paypal completed payment'),
      'group' => t('PayPal'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
    'uc_paypal_event_denied' => array(
      'label' => t('Paypal denied payment'),
      'group' => t('PayPal'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
    'uc_paypal_event_expired' => array(
      'label' => t('Paypal authorization expired'),
      'group' => t('PayPal'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
    'uc_paypal_event_failed' => array(
      'label' => t('Paypal attempt failed'),
      'group' => t('PayPal'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
    'uc_paypal_event_pending' => array(
      'label' => t('Paypal payment is pending'),
      'group' => t('PayPal'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
    'uc_paypal_event_refunded' => array(
      'label' => t('Paypal payment refunded'),
      'group' => t('PayPal'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
    'uc_paypal_event_reversed' => array(
      'label' => t('Paypal reversed a payment'),
      'group' => t('PayPal'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
    'uc_paypal_event_processed' => array(
      'label' => t('Paypal accepted a payment'),
      'group' => t('PayPal'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
    'uc_paypal_event_voided' => array(
      'label' => t('Paypal authorization was voided'),
      'group' => t('PayPal'),
      'variables' => array(
        'order' => array(
          'type' => 'uc_order',
          'label' => t('Order'),
        ),
      ),
    ),
    'uc_paypal_event_verification_fail' => array(
      'label' => t('Paypal IPN transaction failed'),
      'group' => t('PayPal'),
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
