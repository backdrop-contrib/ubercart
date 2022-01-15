<?php
/**
 * @file
 * Token hooks for the uc_cart module.
 */

/**
 * Implements hook_token_info().
 */
function uc_cart_token_info() {
  $item['qty'] = array(
    'name' => t('Quantity'),
    'description' => t('The number of this product in the cart.'),
  );
  $item['node'] = array(
    'name' => t('Node'),
    'description' => t('The node representing the product.'),
  );
  $type = array(
    'name' => t('Cart item'),
    'description' => t('Tokens related to the "Cart item" entities.'),
    'needs-data' => 'uc_cart_item',
  );

  return array(
    'tokens' => array('uc_cart_item' => $item),
    'types' => array('uc_cart_item' => $type),
  );
}

/**
 * Implements hook_tokens().
 */
function uc_cart_tokens($type, $tokens, $data = array(), $options = array()) {
  $values = array();

  if ($type == 'uc_cart_item' && !empty($data['uc_cart_item'])) {
    $item = $data['uc_cart_item'];

    if (isset($tokens['qty'])) {
      $original = $tokens['qty'];
      $values[$original] = $item->qty;
    }

    if (isset($tokens['node'])) {
      $original = $tokens['node'];
      $node = node_load($item->nid);
      $values[$original] = $node->title;
    }

    // Chained token relationships.
    if ($node_tokens = token_find_with_prefix($tokens, 'node')) {
      $node = node_load($item->nid);
      $values += token_generate('node', $node_tokens, array('node' => $node), $options);
    }
  }

  return $values;
}
