<?php


/**
 * Displays customer administration page.
 */
function uc_order_customers($message = NULL, $query = NULL, $page_length = 25) {
  $address = variable_get('uc_customer_list_address', 'billing');
  if ($address == 'shipping') {
    $address = 'delivery';
  }

  if (is_null($query)) {
    $query = uc_order_customer_query($page_length);
  }

  $header = array(
    t('View'),
    array('data' => t('Name'), 'field' => 'o.' . $address . '_last_name', 'sort' => 'asc'),
    array('data' => t('E-mail'), 'field' => 'u.mail'),
    array('data' => t('City'), 'field' => 'o.' . $address . '_city'),
    array('data' => t('ID'), 'field' => 'o.uid'),
  );
  $rows = array();

  $query = $query->extend('TableSort')
    ->orderByHeader($header);

  $result = $query->execute();

  foreach ($result as $customer) {
    $icons = l(uc_store_get_icon('admin/store/customers', TRUE),
               'user/' . $customer->uid,
               array('html' => TRUE, 'attributes' => array('title' => t('View user details.')))
             )
           . l(uc_store_get_icon('admin/store/orders', TRUE),
               'admin/store/customers/orders/' . $customer->uid,
               array('html' => TRUE, 'attributes' => array('title' => t("View customer's order.")))
             );
    if ($address == 'delivery') {
      $name = ucfirst($customer->delivery_last_name) . ', '
            . ucfirst($customer->delivery_first_name);
      $city = ucfirst($customer->delivery_city) . ', '
            . uc_get_zone_code($customer->delivery_zone);
    }
    else {
      $name = ucfirst($customer->billing_last_name) . ', '
            . ucfirst($customer->billing_first_name);
      $city = ucfirst($customer->billing_city) . ', '
            . uc_get_zone_code($customer->billing_zone);
    }
    if ($name == ', ') {
      $name = db_query("SELECT name FROM {users} WHERE uid = :uid", array(':uid' => $customer->uid))->fetchField();
      $name = t('User: !name', array('!name' => $name));
    }
    $rows[] = array(
      'data' => array(
        array('data' => $icons),
        array('data' => check_plain($name)),
        array('data' => check_plain($customer->mail)),
        array('data' => check_plain($city)),
        array('data' => $customer->uid),
      ),
      'id' => 'customer-' . $customer->uid,
    );
  }

  drupal_add_js(drupal_get_path('module', 'uc_order') . '/uc_order.customers.js');

  $build = array();

  $build['customers'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('width' => '100%', 'class' => array('uc-customer-table')),
  );
  $build['pager'] = array(
    '#theme' => 'pager',
    '#weight' => 5,
  );

  return $build;
}

/**
 * Assembles a query to list store customers.
 */
function uc_order_customer_query($page_length = 25) {
  $address = variable_get('uc_customer_list_address', 'billing');
  if ($address == 'shipping') {
    $address = 'delivery';
  }

  $query = db_select('uc_orders', 'o');
  $query->leftJoin('users', 'u', 'o.uid = u.uid');

  $o_uid = $query->addField('o', 'uid');
  $o_status = $query->addField('o', 'order_status');
  $query->fields('o', array(
      $address . '_first_name',
      $address . '_last_name',
      $address . '_city',
      $address . '_zone',
      $address . '_country'
    ))
    ->addField('u', 'mail');

  $query->condition('o.uid', 0, '>')
    ->condition('o.order_status', uc_order_status_list('general', TRUE), 'IN');

  $query = $query->extend('PagerDefault');
  $query->distinct()
    ->limit($page_length);

  return $query;
}

/**
 * Displays the customer search page.
 */
function uc_order_customer_search() {
  $build = array();
  $build['form'] = drupal_get_form('uc_order_customer_search_form');

  if (arg(4) == 'results') {
    $address = variable_get('uc_customer_list_address', 'billing');
    if ($address == 'shipping') {
      $address = 'delivery';
    }

    $query = uc_order_customer_query();

    $first_name = str_replace('*', '%', db_like(arg(5)));
    $last_name = str_replace('*', '%', db_like(arg(6)));
    $email = str_replace('*', '%', db_like(arg(7)));

    if ($first_name !== '0' && $first_name !== '%') {
      $query->condition('o.' . $address . '_first_name', $first_name, 'LIKE');
    }
    if ($last_name !== '0' && $last_name !== '%') {
      $query->condition('o.' . $address . '_last_name', $last_name, 'LIKE');
    }
    if ($email !== '0' && $email !== '%') {
      $query->addField('o', 'primary_email');
      $query->condition('o.primary_email', $email, 'LIKE');
    }

    $message = t('Search returned the following results:');

    $build['results'] = uc_order_customers($message, $query, 100);
  }

  return $build;
}

/**
 * Form builder for the customer search form.
 *
 * @see uc_order_customer_search_form_submit()
 * @ingroup forms
 */
function uc_order_customer_search_form($form, &$form_state) {
  $form['search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search options'),
    '#description' => t('Search for customers based on any of the following fields. Use * as a wildcard to match any character.') . '<br />' . t("For example, searching by last name for 's*' will return all customers whose last name starts with an s.") . '<br />' . t('(<em>Leave a field empty to ignore it in the search.</em>)'),
    '#collapsible' => TRUE,
    '#collapsed' => arg(4) == 'results' ? TRUE : FALSE,
    '#theme' => 'uc_order_customer_search_fieldset',
  );

  $form['search']['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
    '#default_value' => arg(5) != '0' ? arg(5) : '',
    '#size' => 24,
    '#maxlength' => 32,
  );

  $form['search']['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
    '#default_value' => arg(6) != '0' ? arg(6) : '',
    '#size' => 24,
    '#maxlength' => 32,
  );

  $form['search']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail'),
    '#default_value' => arg(7) != '0' ? arg(7) : '',
    '#size' => 24,
    '#maxlength' => 96,
  );

  $form['search']['actions'] = array('#type' => 'actions');
  $form['search']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );

  return $form;
}

/**
 * Returns HTML for the customer search fieldset
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @ingroup themeable
 */
function theme_uc_order_customer_search_fieldset($variables) {
  $form = $variables['form'];

  $output = '<table><tbody style="border: 0px;"><tr><td>';
  $output .= drupal_render($form['first_name']);

  $output .= '</td><td>';
  $output .= drupal_render($form['last_name']);

  $output .= '</td><td>';
  $output .= drupal_render($form['email']);

  $output .= '</td><td>';
  $output .= drupal_render($form['submit']);

  $output .= '</td></tr></tbody></table>';
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Form submission handler for uc_order_customer_search_form().
 *
 * @see uc_order_customer_search_form()
 */
function uc_order_customer_search_form_submit($form, &$form_state) {
  if (strlen(trim($form_state['values']['first_name'])) == 0) {
    $first_name = '0';
  }
  else {
    $first_name = drupal_strtolower(trim($form_state['values']['first_name']));
  }

  if (strlen(trim($form_state['values']['last_name'])) == 0) {
    $last_name = '0';
  }
  else {
    $last_name = drupal_strtolower(trim($form_state['values']['last_name']));
  }

  if (strlen(trim($form_state['values']['email'])) == 0) {
    $email = '0';
  }
  else {
    $email = drupal_strtolower(trim($form_state['values']['email']));
  }

  drupal_goto('admin/store/customers/search/results/' . $first_name . '/'
            . $last_name . '/' . $email);
}

/**
 * Displays a list of orders made by a customer.
 *
 * $param $uid
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
    $icons = l(uc_store_get_icon('file:order_view'),
               'admin/store/orders/' . $order->order_id,
               array('html' => TRUE, 'attributes' => array('title' => t("View order !order_id.", array('!order_id' => $order->order_id))))
             )
           . l(uc_store_get_icon('file:order_edit'),
               'admin/store/orders/' . $order->order_id . '/edit',
               array('html' => TRUE, 'attributes' =>
                 array(
                   'title' => t("Edit order !order_id.",
                     array('!order_id' => $order->order_id)
                   )
                 )
               )
             );
    $bname = ucfirst($order->billing_first_name) . ' '
           . ucfirst($order->billing_last_name);
    $sname = ucfirst($order->delivery_first_name) . ' '
           . ucfirst($order->delivery_last_name);
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

  drupal_add_js(drupal_get_path('module', 'uc_order') . '/uc_order.customers.js');

  $build = array();

  $build['description'] = array(
    '#markup' => '<p>' . t('!totals_orders orders shown match that account with !totals_items items purchased and !totals_total spent:',
       array(
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
