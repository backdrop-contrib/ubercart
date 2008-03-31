<?php
// $Id$

/**
 * @file
 * This file contains all the Workflow-NG hooks that are neccesary for Workflow
 * integeration with the uc_stock module
 */


/* ************************************************************************* *
 *  Workflow-ng Hooks                                                        *
 * ************************************************************************* */

/*
 * Implementation of hook_configuration
 */
function uc_stock_configuration() {
  $configurations = array();
  $configurations['uc_stock_decrement_on_order'] = array(
    '#label' => t('Decrement stock upon order submission'),
    '#event' => 'checkout_complete',
    '#module' => 'uc_stock',
  );
  $action = workflow_ng_use_action('uc_stock_action_decrement_stock', array(
    '#label' => t('Decrement stock of products in order'),
    '#arguments' => array(
      'order' => array('#entity' => 'order', '#label' => t('Order')),
    ),
  ));
  $configurations['uc_stock_decrement_on_order'] = workflow_ng_configure($configurations['uc_stock_decrement_on_order'], $action);
  return $configurations;
}

/*
 * Implementation of hook_action_info()
 */
function uc_stock_action_info() {
  return array(
    'uc_stock_action_decrement_stock' => array(
      '#label' => t('Decrement stock of products in order'),
      '#arguments' => array(
        'order' => array('#entity' => 'order', '#label' => t('Order')),
      ),
      '#module' => t('Stock'),
    ),
  );
}

/* ************************************************************************* *
 *  Workflow-ng Action Callbacks and Forms                                   *
 * ************************************************************************* */

function uc_stock_action_decrement_stock($order, $settings) {
  if (is_array($order->products)) {
    $stock_warnings = array();
    foreach ($order->products as $product) {
      if (($stock = uc_stock_level($product->model)) !== FALSE) {
        $stock_level = db_fetch_object(db_query("SELECT * FROM {uc_product_stock} WHERE sku = '%s'", $product->model));
        if ((($stock - $product->qty) <= $stock_level->threshold) && !in_array($product->model, array_keys($stock_warnings))) {
          $stock_level->stock -= $product->qty;
          $stock_warnings[$product->model] = $stock_level;
        }
        uc_stock_adjust($product->model, -$product->qty);
        uc_order_comment_save($order->order_id, 0, t('The stock level for %model_name has been decreased to !qty.', array('%model_name' => $product->model, '!qty' => ($stock - $product->qty))));
      }
    }
    if (!empty($stock_warnings) && variable_get('uc_stock_threshold_notification', FALSE)) {
      foreach ($stock_warnings as $model => $stock_level) {
        _uc_stock_send_mail($order, $stock_level);
      }
    }
  }
}
