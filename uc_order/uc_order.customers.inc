<?php

/**
 * @file
 * Helper functions and forms for administering customers.
 */


/**
 * Displays a list of orders made by a customer.
 *
 * @param $uid
 *   The user id of the customer.
 */
function uc_order_customer_orders($uid) {
  $result = db_select('uc_orders')->extend('PagerDefault')
    ->fields('uc_orders')
    ->condition('uid', $uid)
    ->condition('order_status', uc_order_status_list('general', TRUE), 'IN')
    ->orderBy('created', 'DESC')
    ->limit(50)
    ->execute();

  $header = array(t('View'), t('Order ID'), t('Date'), t('Billing name'),
                  t('Shipping name'), t('Items'), t('Total'));
  $rows = array();
  $totals = array('orders' => 0, 'items' => 0, 'total' => 0);
  foreach ($result as $order) {
    $icons = l(uc_store_get_icon('file:order_view'), 'admin/store/orders/' . $order->order_id, array(
      'html' => TRUE,
      'attributes' => array('title' => t("View order !order_id.", array('!order_id' => $order->order_id))),
    ));
    $icons .= l(uc_store_get_icon('file:order_edit'), 'admin/store/orders/' . $order->order_id . '/edit', array(
      'html' => TRUE,
      'attributes' => array('title' => t("Edit order !order_id.", array('!order_id' => $order->order_id))),
    ));
    $bname = drupal_ucfirst($order->billing_first_name) . ' ' . drupal_ucfirst($order->billing_last_name);
    $sname = drupal_ucfirst($order->delivery_first_name) . ' ' . drupal_ucfirst($order->delivery_last_name);
    $item_count = db_query("SELECT COUNT(*) FROM {uc_order_products} WHERE order_id = :order_id", array(':order_id' => $order->order_id))->fetchField();

    $totals['orders'] += 1;
    $totals['items'] += $item_count;
    $totals['total'] += $order->order_total;

    $rows[] = array(
      'data' => array(
        array('data' => $icons),
        array('data' => $order->order_id),
        array('data' => format_date($order->created, 'uc_store')),
        array('data' => check_plain($bname)),
        array('data' => check_plain($sname)),
        array('data' => $item_count['COUNT(*)']),
        array('data' => array('#theme' => 'uc_price', '#price' => $order->order_total)),
      ),
      'id' => 'order-' . $order->order_id,
    );
  }

  drupal_add_js(drupal_get_path('module', 'uc_order') . '/uc_order.js');

  $build = array();

  $build['description'] = array(
    '#markup' => '<p>' . t('!totals_orders orders shown match that account with !totals_items items purchased and !totals_total spent:', array(
      '!totals_orders' => $totals['orders'],
      '!totals_items' => $totals['items'],
      '!totals_total' => uc_currency_format($totals['total']),
    )) . '</p>',
    '#weight' => -5,
  );
  $build['orders'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('width' => '100%', 'class' => array('uc-cust-orders-table')),
    '#empty' => t('No orders found.'),
  );
  $build['pager'] = array(
    '#theme' => 'pager',
    '#weight' => 5,
  );

  return $build;
}
