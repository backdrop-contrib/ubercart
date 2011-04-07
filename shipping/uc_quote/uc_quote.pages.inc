<?php

/**
 * @file
 * Menu callbacks for shipping quotes requested through AJAX.
 */

/**
 * Pulls the get_quote_from_* triggers and assemble their returned data.
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

  $quote_data = array();
  foreach ($methods as $method) {
    if (module_exists('rules')) {
      $set = rules_config_load('get_quote_from_' . $method['id']);
      $apply = !$set || $set->execute($order);
    }
    else {
      $apply = TRUE;
    }

    if ($apply) {
      $data = uc_quote_action_get_quote($order, $method);

      foreach ($data as &$quote) {
        if (isset($quote['rate'])) {
          $quote['format'] = uc_currency_format($quote['rate']);
        }
      }
      $quote_data[$method['id']] = $data;
    }
  }
  return $quote_data;
}

/**
 * Retrieves shipping quote.
 *
 * @param $order
 *   The order the quote is for.
 * @param $method
 *   The shipping method to generate the quote.
 *
 * @return
 *   Array of shipping quotes.
 */
function uc_quote_action_get_quote($order, $method) {
  $details = array();
  foreach ($order as $key => $value) {
    if (substr($key, 0, 9) == 'delivery_') {
      $field = substr($key, 9);
      $details[$field] = $value;
    }
  }
  ob_start();
  // Load include file containing quote callback, if there is one
  if (isset($method['quote']['file'])) {
    $inc_file = drupal_get_path('module', $method['module']) . '/' . $method['quote']['file'];
    if (is_file($inc_file)) {
      require_once($inc_file);
    }
  }

  if (function_exists($method['quote']['callback'])) {
    // This feels wrong, but it's the only way I can ensure that shipping
    // methods won't mess up the products in their methods.
    $products = array();
    foreach ($order->products as $key => $item) {
      if (uc_cart_product_is_shippable($item)) {
        $products[$key] = clone $item;
      }
    }
    $quote_data = call_user_func($method['quote']['callback'], $products, $details, $method);
  }
  $messages = ob_get_contents();
  ob_end_clean();

  if ($messages && variable_get('uc_quote_log_errors', FALSE)) {
    watchdog('quote', '!messages', array('!messages' => $messages), WATCHDOG_WARNING);
    watchdog('quote', '<pre>@data</pre>', array('@data' => print_r($quote_data, TRUE)), WATCHDOG_WARNING);
  }
  return $quote_data;
}
