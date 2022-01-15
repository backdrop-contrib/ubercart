<?php
/**
 * @file
 * Rules hooks for uc_stock.module.
 */

/**
 * Implements hook_rules_action_info().
 */
function uc_stock_rules_action_info() {
  $actions['uc_stock_action_adjust_product_stock'] = array(
    'label' => t('Adjust stock of a product with tracking activated.'),
    'group' => t('Stock'),
    'base' => 'uc_stock_action_adjust_product_stock',
    'parameter' => array(
      'order' => array(
        'type' => 'uc_order',
        'label' => t('Order'),
      ),
      'product' => array(
        'type' => 'uc_order_product',
        'label' => t('Product'),
      ),
    ),
  );

  $actions['uc_stock_action_decrement_stock'] = array(
    'label' => t('Decrement stock of products with tracking activated on the order.'),
    'group' => t('Stock'),
    'base' => 'uc_stock_action_decrement_stock',
    'parameter' => array(
      'order' => array(
        'type' => 'uc_order',
        'label' => t('Order'),
      ),
    ),
  );

  $actions['uc_stock_action_increment_stock'] = array(
    'label' => t('Increment stock of products with tracking activated on the order.'),
    'group' => t('Stock'),
    'base' => 'uc_stock_action_increment_stock',
    'parameter' => array(
      'order' => array(
        'type' => 'uc_order',
        'label' => t('Order'),
      ),
    ),
  );

  return $actions;
}

/**
 * Decrease the stock of a product by the amount in $product->qty.
 */
function uc_stock_action_adjust_product_stock($order, $product) {
  uc_stock_adjust_product_stock($product, 0, $order);
}

/**
 * Decrease the stock of ordered products.
 */
function uc_stock_action_decrement_stock($order) {
  if (is_array($order->products)) {
    array_walk($order->products, 'uc_stock_adjust_product_stock', $order);
  }
}

/**
 * Increase the stock of ordered products.
 */
function uc_stock_action_increment_stock($order, $settings) {
  if (is_array($order->products)) {
    array_walk($order->products, 'uc_stock_increment_product_stock', $order);
  }
}
