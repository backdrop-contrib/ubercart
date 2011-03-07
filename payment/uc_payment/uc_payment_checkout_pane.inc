<?php

/**
 * @file
 * Checkout pane functions for uc_payment.module.
 *
 * The checkout pane holds form to select the payment method. It also shows a
 * preview of the line items and order total.
 */

function uc_checkout_pane_payment($op, &$order, $form = NULL, &$form_state = NULL) {
  switch ($op) {
    case 'view':
      if (empty($order->line_items)) {
        $order->line_items = uc_order_load_line_items($order, FALSE);
      }

      if (variable_get('uc_payment_show_order_total_preview', TRUE)) {
        $contents['current_total'] = array(
          '#type' => 'hidden',
          '#value' => isset($order->order_total) && $order->order_total > 0 ? $order->order_total : NULL,
        );
        $contents['line_items'] = array(
          '#theme' => 'uc_payment_totals',
          '#order' => $order,
          '#prefix' => '<div style="padding: .5em 1em; margin-bottom: 1em; border: dashed 1px #bbb;" id="line-items-div">',
          '#suffix' => '</div>',
          '#weight' => -20,
        );
      }

      $methods = _uc_payment_method_list();
      $options = array();
      $default = NULL;

      foreach ($methods as $method) {
        if ($method['checkout']) {
          $options[$method['id']] = $method['title'];
        }
      }

      if (count($options)) {
        if (isset($form_state['values'])  &&
          isset($form_state['values']['panes']['payment']['payment_method']) &&
          in_array($form_state['values']['panes']['payment']['payment_method'], array_keys($options))) {
          $default = $form_state['values']['panes']['payment']['payment_method'];
        }
        else {
          $default = (count($options) == 1 || empty($order->payment_method)) ? key($options) : $order->payment_method;
        }
      }

      if (count($options) > 1) {
        $description = t('Select a payment method from the following options.');
      }
      else {
        $description = '';
      }

      $contents['payment_method'] = array(
        '#type' => 'radios',
        '#title' => t('Payment method'),
        '#options' => $options,
        '#default_value' => $default,
        '#disabled' => count($options) == 1 ? TRUE : FALSE,
        '#required' => TRUE,
        '#theme' => 'uc_payment_method_select',
        '#ajax' => array(
          'callback' => 'uc_payment_checkout_payment_details',
          'wrapper' => 'payment_details',
        ),
      );

      $contents['details'] = array(
        '#prefix' => '<div id="payment_details" class="solid-border">',
        '#suffix' => '</div>',
      );

      $func = _uc_payment_method_data($default, 'callback');
      if (function_exists($func)) {
        $details = $func('cart-details', $order, $form, $form_state);
        if (is_array($details) && !empty($details)) {
          $contents['details'] += $details;
        }
      }

      return array('description' => $description, 'contents' => $contents);

    case 'process':
      $order->payment_method = $form_state['values']['panes']['payment']['payment_method'];
      $func = _uc_payment_method_data($order->payment_method, 'callback');
      if (function_exists($func)) {
        $result = $func('cart-process', $order, $form, $form_state);
        if ($result === FALSE) {
          return FALSE;
        }
      }
      return TRUE;

    case 'review':
      $line_items = $order->line_items;
      $items = _uc_line_item_list();
      foreach ($items as $item) {
        if (isset($item['display_only']) && $item['display_only'] == TRUE) {
          $result = $item['callback']('display', $order);
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
        $review[] = array('title' => $line_item['title'], 'data' => theme('uc_price', array('price' => $line_item['amount'])));
      }
      $review_data = _uc_payment_method_data($order->payment_method, 'review');
      if (empty($review_data)) {
        $review_data = _uc_payment_method_data($order->payment_method, 'name');
      }
      $review[] = array('border' => 'top', 'title' => t('Paying by'), 'data' => $review_data);
      $func = _uc_payment_method_data($order->payment_method, 'callback');
      if (function_exists($func)) {
        $result = $func('cart-review', $order);
        if (is_array($result)) {
          $review = array_merge($review, $result);
        }
      }
      return $review;

    case 'settings':
      $form['uc_payment_show_order_total_preview'] = array(
        '#type' => 'checkbox',
        '#title' => t('Show the order total preview on the payment pane.'),
        '#default_value' => variable_get('uc_payment_show_order_total_preview', TRUE),
      );
      return $form;
  }
}

/**
 * AJAX callback for payment method details on the checkout form.
 */
function uc_payment_checkout_payment_details($form, $form_state) {
  return $form['panes']['payment']['details'];
}

/**
 * We need a theme function for the radios element in case another module alters
 * the default or available payment methods... we need the JS to grab the right
 * default payment details.
 *
 * @ingroup themeable
 */
function theme_uc_payment_method_select($variables) {
  $form = $variables['form'];

  if (empty($form['#options'])) {
    drupal_set_message(t('Checkout cannot be completed without any payment methods enabled. Please contact an administrator to resolve the issue.'), 'error');
    return;
  }

  return drupal_render_children($form);
}
