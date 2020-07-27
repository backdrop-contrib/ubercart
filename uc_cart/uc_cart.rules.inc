<?php

/**
 * @file
 * Hooks and functions for uc_cart Rules integration.
 */

/**
 * Implements hook_rules_event_info().
 */
function uc_cart_rules_event_info() {
  $events['uc_checkout_complete'] = array(
    'label' => t('Customer completes checkout'),
    'group' => t('Cart'),
    'variables' => array(
      'order' => array(
        'type' => 'uc_order',
        'label' => t('Order'),
      ),
    ),
  );

  $events['uc_cart_checkout_start'] = array(
    'label' => t('Customer starts checkout'),
    'group' => t('Cart'),
    'variables' => array(
      'order' => array(
        'type' => 'uc_order',
        'label' => t('Order'),
      ),
    ),
  );

  return $events;
}
