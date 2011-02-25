<?php

/**
 * @file
 * Token hooks
 */

/**
 * Implements hook_token_info().
 */
function uc_order_token_info() {
  $type = array(
    'name' => t('Orders'),
    'description' => t('Tokens related to Ubercart orders.'),
    'needs-data' => 'uc_order',
  );

  $order['new-username'] = array(
    'name' => t('New username'),
    'description' => t('New username associated with an order if applicable.'),
  );
  $order['new-password'] = array(
    'name' => t('New password'),
    'description' => t('New password associated with an order if applicable.'),
  );
  $order['order-id'] = array(
    'name' => t('Order ID'),
    'description' => t('The unique identifier of the order.'),
  );
  $order['url'] = array(
    'name' => t('URL'),
    'description' => t('The URL to the order'),
  );
  $order['link'] = array(
    'name' => t('Link'),
    'description' => t('A link to the order using the order ID.'),
  );
  $order['admin-url'] = array(
    'name' => t('Admin URL'),
    'description' => t('The URL to the admin view page using the order ID.'),
  );
  $order['admin-link'] = array(
    'name' => t('Admin link'),
    'description' => t('A link to the order admin view page using the order ID.'),
  );
  $order['subtotal'] = array(
    'name' => t('Subtotal'),
    'description' => t('The subtotal of products on an order.'),
  );
  $order['total'] = array(
    'name' => t('Total'),
    'description' => t('The order total.'),
  );
  $order['email'] = array(
    'name' => t('Email'),
    'description' => t('The primary e-mail address of the order.'),
  );
  $order['shipping-method'] = array(
    'name' => t('Shipping method'),
    'description' => t('The title of the first shipping line item.'),
  );
  $order['shipping-address'] = array(
    'name' => t('Shipping address'),
    'description' => t('The order shipping address.'),
  );
  $order['shipping-phone'] = array(
    'name' => t('Shipping phone number'),
    'description' => t('The phone number for the shipping address.'),
  );
  $order['billing-address'] = array(
    'name' => t('Billing address'),
    'description' => t('The order billing address.'),
  );
  $order['billing-phone'] = array(
    'name' => t('Billing phone number'),
    'description' => t('The phone number for the billing address.'),
  );
  $order['first-name'] = array(
    'name' => t("Customer's first name"),
    'description' => t('The first name associated with the order.'),
  );
  $order['last-name'] = array(
    'name' => t("Customer's last name"),
    'description' => t('The last name associated with the order.'),
  );
  $order['comments'] = array(
    'name' => t('Comments'),
    'description' => t('Comments left by the customer.'),
  );
  $order['last-comment'] = array(
    'name' => t('Last comment'),
    'description' => t('Last order comment left by an administrator (not counting the order admin comments).'),
  );
  $order['order-status'] = array(
    'name' => t('Order status'),
    'description' => t('The current order status.'),
  );

  $order['customer'] = array(
    'name' => t('Customer'),
    'description' => t('The user associated with the order.'),
    'type' => 'user',
  );
  $order['created'] = array(
    'name' => t('Created'),
    'description' => t('The date and time when the order was created.'),
    'type' => 'date',
  );
  $order['modified'] = array(
    'name' => t('Modified'),
    'description' => t('The date and time when the order was last modified.'),
    'type' => 'date',
  );

  return array(
    'types' => array('uc_order' => $type),
    'tokens' => array('uc_order' => $order),
  );
}

/**
 * Implements hook_tokens().
 */
function uc_order_tokens($type, $tokens, $data = array(), $options = array()) {
  $values = array();

  $sanitize = !empty($options['sanitize']);

  if ($type == 'uc_order' && !empty($data['uc_order'])) {
    $order = $data['uc_order'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'new-username':
          if (isset($_SESSION['new_user']) && is_array($_SESSION['new_user'])) {
            $values[$original] = $sanitize ? check_plain($_SESSION['new_user']['name']) : $_SESSION['new_user']['name'];
          }
          break;

        case 'new-password':
          if (isset($_SESSION['new_user']) && is_array($_SESSION['new_user'])) {
            $values[$original] = $sanitize ? check_plain($_SESSION['new_user']['pass']) : $_SESSION['new_user']['pass'];
          }
          break;

        case 'order-id':
          $values[$original] = $order->order_id;
          break;

        case 'url':
        case 'link':
          if (!isset($url)) {
            $url = url('user/' . $order->uid . '/orders/' . $order->order_id, array('absolute' => TRUE));
          }
          else {
            break;
          }

          if (isset($tokens['url'])) {
            $values[$tokens['url']] = $url;
          }
          if (isset($tokens['link'])) {
            $values[$tokens['link']] = l($order->order_id, $url);
          }
          break;

        case 'admin-url':
        case 'admin-link':
          if (!isset($admin_url)) {
            $admin_url = url('admin/store/orders/' . $order->order_id, array('absolute' => TRUE));
          }
          else {
            break;
          }

          if (isset($tokens['admin-url'])) {
            $values[$tokens['admin-url']] = $admin_url;
          }
          if (isset($tokens['admin-link'])) {
            $values[$tokens['admin-link']] = l($order->order_id, $admin_url);
          }
          break;

        case 'subtotal':
          if (is_array($order->line_items)) {
            foreach ($order->line_items as $key => $value) {
              if ($value['type'] == 'subtotal') {
                $subtotal = uc_currency_format($order->line_items[$key]['amount']);
              }
              if ($value['type'] == 'shipping' && !isset($ship_method)) {
                $ship_method = $value['title'];
              }
            }
          }
          $values[$original] = $subtotal;
          break;

        case 'total':
          $values[$original] = uc_currency_format($order->order_total);
          break;

        case 'email':
          $values[$original] = $sanitize ? check_plain($order->primary_email) : $order->primary_email;
          break;

        case 'shipping-method':
          if (is_array($order->line_items)) {
            foreach ($order->line_items as $key => $value) {
              if ($value['type'] == 'shipping' && !isset($ship_method)) {
                $ship_method = $value['title'];
                break;
              }
            }
          }
          $values[$original] = !isset($ship_method) ? t('Standard delivery') : $ship_method;
          break;

        case 'shipping-address':
          $values[$original] = uc_order_address($order, 'delivery');
          break;

        case 'shipping-phone':
          $values[$original] = check_plain($order->delivery_phone);
          break;

        case 'billing-address':
          $values[$original] = uc_order_address($order, 'billing');
          break;

        case 'billing-phone':
          $values[$original] = check_plain($order->billing_phone);
          break;

        case 'first-name':
        case 'last-name':
          if (variable_get('uc_customer_list_address', 'billing') == 'shipping') {
            $values[$tokens['first-name']] = $sanitize ? check_plain($order->delivery_first_name) : $order->delivery_first_name;
            $values[$tokens['last-name']] = $sanitize ? check_plain($order->delivery_last_name) : $order->delivery_last_name;
          }
          else {
            $values[$tokens['first-name']] = $sanitize ? check_plain($order->billing_first_name) : $order->billing_first_name;
            $values[$tokens['last-name']] = $sanitize ? check_plain($order->billing_last_name) : $order->billing_last_name;
          }
          break;

        case 'comments':
          $result = db_query_range("SELECT message FROM {uc_order_comments} WHERE order_id = :order_id AND uid = :uid ORDER BY created DESC", 0, 1, array(':order_id' => $order->order_id, ':uid' => 0))->fetchField();
          if ($sanitize) {
            $result = check_plain($result);
          }
          $values[$original] = empty($result) ? t('<i>No comments left.</i>') : $result;
          break;

        case 'last-comment':
          $result = db_query_range("SELECT message FROM {uc_order_comments} WHERE order_id = :order_id AND uid > :uid ORDER BY created DESC", 0, 1, array(':order_id' => $order->order_id, ':uid' => 0))->fetchField();
          if ($sanitize) {
            $result = check_plain($result);
          }
          $values[$original] = empty($result) ? t('<i>No comment found.</i>') : $result;
          break;

        case 'order-status':
          $values[$original] = uc_order_status_data($order->order_status, 'title');
          break;

        case 'customer':
          $values[$original] = $order->uid;
          break;

        case 'created':
          $values[$original] = format_date($order->created, 'short');
          break;

        case 'modified':
          $values[$original] = format_date($order->modified, 'short');
          break;
      }
    }

    if ($customer_tokens = token_find_with_prefix($tokens, 'customer')) {
      $customer = user_load($order->uid);
      $values += token_generate('user', $customer_tokens, array('user' => $customer), $options);
    }

    if ($created_tokens = token_find_with_prefix($tokens, 'created')) {
      $values += token_generate('date', $created_tokens, array('date' => $order->created), $options);
    }

    if ($changed_tokens = token_find_with_prefix($tokens, 'modified')) {
      $values += token_generate('date', $changed_tokens, array('date' => $order->modified), $options);
    }
  }

  return $values;
}
