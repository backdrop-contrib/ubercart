<?php
// $Id$

/**
 * @file
 * Menu callbacks for shipping quotes requested through AJAX.
 */

/**
 * Pull the get_quote_from_* triggers and assemble their returned data.
 */
function uc_quote_assemble_quotes($order) {
  global $user;

  if (!$order->uid || !($account = user_load($order->uid))) {
    $account = $user;
  }

  $products = $order->products;
  $shipping_types = array();
  foreach ($products as $product) {
    $shipping_types[] =  uc_product_get_shipping_type($product);
  }
  $shipping_types = array_unique($shipping_types);
  $all_types = uc_quote_get_shipping_types();
  $shipping_type = '';

  // Use the most prominent shipping type (highest weight).
  // In theory, you can ship lighter products with heavier by the same
  // method, but not vice versa.
  $type_weight = -1000; // arbitrary low number
  foreach ($shipping_types as $type) {
    if ($all_types[$type]['weight'] > $type_weight) {
      $shipping_type = $all_types[$type]['id'];
      $type_weight = $all_types[$type]['weight'];
    }
  }
  $methods = array_filter(module_invoke_all('uc_shipping_method'), '_uc_quote_method_enabled');
  uasort($methods, '_uc_quote_type_sort');
  foreach ($methods as $id => $method) {
    if ($method['quote']['type'] != 'order' && $method['quote']['type'] != $shipping_type) {
      unset($methods[$id]);
    }
  }

  $context = array(
    'revision' => 'formatted',
    'type' => 'line_item',
    'subject' => array(
      'order' => $order,
    ),
  );

  $quote_data = array();
  $arguments = array(
    'order' => array(
      '#entity' => 'uc_order',
      '#title' => t('Order'),
      '#data' => $order,
    ),
    'method' => array(
      '#entity' => 'quote_method',
      '#title' => t('Quote method'),
      // #data => each $method in the following foreach() loop;
    ),
    'account' => array(
      '#entity' => 'user',
      '#title' => t('User'),
      '#data' => $account,
    ),
  );
  foreach ($methods as $method) {
    $arguments['method']['#data'] = $method;
    $predicates = ca_load_trigger_predicates('get_quote_from_' . $method['id']);
    $predicate = array_shift($predicates);
    if ($predicate && ca_evaluate_conditions($predicate, $arguments)) {
      $data = uc_quote_action_get_quote($order, $method, $user);

      foreach ($data as &$quote) {
        if (isset($quote['rate'])) {
          $context['subject']['line_item'] = array(
            'type' => 'shipping',
            'name' => $quote['option_label'],
            'amount' => $quote['rate'],
            'weight' => 1,
          );

          $quote['format'] = uc_price($quote['rate'], $context);
        }
      }
      $quote_data[$method['id']] = $data;
    }
  }
  return $quote_data;
}

