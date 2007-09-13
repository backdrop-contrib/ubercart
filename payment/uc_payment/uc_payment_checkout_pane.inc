<?php
// $Id$

function uc_checkout_pane_payment($op, &$arg1, $arg2) {
  switch ($op) {
    case 'view':
      if (module_exists('uc_taxes')){
        drupal_add_js('var tax_weight = '. variable_get('uc_li_tax_weight', 9), 'inline');
        drupal_add_js(drupal_get_path('module', 'uc_taxes') .'/uc_taxes.js');
      }
      $contents['current_total'] = array(
        '#type' => 'hidden',
        '#value' => $arg1->order_total > 0 ? $arg1->order_total : NULL,
      );
      $contents['shown_total'] = array(
        '#value' => '<div style="padding: .5em 1em; margin-bottom: 1em; border: dashed 1px #bbb;" '
                   .'id="line-items-div"><em>'. t('Preparing order total preview...') .'</em></div>',
        '#weight' => -20,
      );
      $description = t('Select a payment method from the following options.');

      $methods = _payment_method_list();
      foreach ($methods as $method) {
        if ($method['checkout']) {
          $options[$method['id']] = $method['title'];
        }
      }
      $contents['payment_method'] = array(
        '#type' => 'radios',
        '#title' => t('Payment Method'),
        '#options' => $options,
        '#default_value' => count($options) == 1 ? array_pop(array_keys($options)) : $arg1->payment_method,
        '#disabled' => count($options) == 1 ? TRUE : FALSE,
        '#required' => TRUE,
        '#attributes' => array('onclick' => "get_payment_details('". base_path() ."?q=cart/checkout/payment_details/' + this.value);"),
      );
      drupal_add_js("\$(document).ready( function () { show_default_payment_details('". base_path() ."?q=cart/checkout/payment_details/'); } );", 'inline');
      $contents['details'] = array(
        '#value' => '<div id="payment_details" class="solid-border display-none"></div>',
      );

      return array('description' => $description, 'contents' => $contents);

    case 'process':
      $arg1->payment_method = $arg2['payment_method'];
      $func = _payment_method_data($arg1->payment_method, 'callback');
      if (function_exists($func)) {
        $result = $func('cart-process', $arg1);
        if ($result === FALSE) {
          return FALSE;
        }
      }
      return TRUE;

    case 'review':
      $line_items = $arg1->line_items;
      $items = _line_item_list();
      foreach ($items as $item) {
        if (isset($item['display_only']) && $item['display_only'] == TRUE) {
          $result = $item['callback']('display', $arg1);
          if (is_array($result)) {
            foreach ($result as $line) {
              $line_items[] = array(
                'title' => $line['title'],
                'amount' => $line['amount'],
                'weight' => $item['weight']
              );
            }
          }
        }
      }
      usort($line_items, 'uc_weight_sort');
      foreach ($line_items as $line_item) {
        $review[] = array('title' => $line_item['title'], 'data' => uc_currency_format($line_item['amount']));
      }
      $review_data = _payment_method_data($arg1->payment_method, 'review');
      if (empty($review_data)) {
        $review_data = _payment_method_data($arg1->payment_method, 'name');
      }
      $review[] = array('border' => 'top', 'title' => t('Paying by'), 'data' => $review_data);
      $func = _payment_method_data($arg1->payment_method, 'callback');
      if (function_exists($func)) {
        $result = $func('cart-review', $arg1);
        if (is_array($result)) {
          $review = array_merge($review, $result);
        }
      }
      return $review;
  }
}

