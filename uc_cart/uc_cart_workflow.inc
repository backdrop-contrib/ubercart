<?php
// $Id$

/**
 * @file
 * This file contains the Workflow-ng hooks and functions necessary to make the
 * cart related entity, conditions, events, and actions work.
 */


/******************************************************************************
 * Workflow-ng Hooks                                                          *
 ******************************************************************************/

// Tell Workflow about the various cart events.
function uc_cart_event_info() {
  $order_arg = array('#entity' => 'order', '#label' => t('Order'));

  $events['checkout_complete'] = array(
    '#label' => t('Customer completes checkout'),
    '#module' => t('Cart'),
    '#arguments' => array('order' => $order_arg),
  );

  return $events;
}
