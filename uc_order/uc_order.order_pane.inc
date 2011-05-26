<?php

/**
 * @file
 * This file contains the callbacks for the default order panes supplied with
 * Ubercart and their corresponding helper functions.
 *
 * Order panes are defined using hook_uc_order_pane() and use a callback to
 * handle the different processes involved in order viewing/editing. The
 * default order panes are defined in uc_order_order_pane() in uc_order.module.
 */

/**
 * Handles the "Ship to" order pane.
 */
function uc_order_pane_ship_to($op, $order, &$form = NULL, &$form_state = NULL) {
  switch ($op) {
    case 'customer':
      if (!uc_order_is_shippable($order)) {
        return;
      }
    case 'view':
      $build = array('#markup' => uc_order_address($order, 'delivery') . '<br />' . check_plain($order->delivery_phone));
      return $build;

    case 'edit-form':
      $form['ship_to'] = array(
        '#type' => 'uc_address',
        '#default_value' => isset($form_state['values']) ? $form_state['values'] : $order,
        '#required' => FALSE,
        '#attributes' => array('class' => array('uc-store-address-field')),
        '#key_prefix' => 'delivery',
      );
      return $form;

    case 'edit-title':
      $output = ' <img src="' . base_path() . drupal_get_path('module', 'uc_store')
                . '/images/address_book.gif" alt="' . t('Select from address book.') . '" '
                . 'title="' . t('Select from address book.') . '" onclick="load_address_select(' . $form['order_uid']['#value'] . ', \'#delivery_address_select\', \'delivery\');" '
                . 'style="position: relative; top: 2px; cursor: pointer;" />';

      $output .= ' <img src="' . base_path() . drupal_get_path('module', 'uc_store')
                 . '/images/copy.gif" alt="' . t('Copy billing information.') . '" title="'
                 . t('Copy billing information.') . '" onclick="uc_order_copy_billing_to_shipping();" '
                 . 'style="position: relative; top: 2px; cursor: pointer;" />';

      return $output;

    case 'edit-theme':
      return drupal_render($form['ship_to']);

    case 'edit-process':
      foreach ($form_state['values'] as $key => $value) {
        if (substr($key, 0, 9) == 'delivery_') {
          if (uc_address_field_enabled(substr($key, 9))) {
            $changes[$key] = $value;
          }
        }
      }
      return $changes;
  }
}

/**
 * Handles the "Bill to" order pane.
 */
function uc_order_pane_bill_to($op, $order, &$form = NULL, &$form_state = NULL) {
  switch ($op) {
    case 'view':
    case 'customer':
      $build = array('#markup' => uc_order_address($order, 'billing') . '<br />' . check_plain($order->billing_phone));
      return $build;

    case 'edit-form':
      $form['bill_to'] = array(
        '#type' => 'uc_address',
        '#default_value' => isset($form_state['values']) ? $form_state['values'] : $order,
        '#required' => FALSE,
        '#attributes' => array('class' => array('uc-store-address-field')),
        '#key_prefix' => 'billing',
      );
      return $form;

    case 'edit-title':
      $output = ' <img src="' . base_path() . drupal_get_path('module', 'uc_store')
                . '/images/address_book.gif" alt="' . t('Select from address book.') . '" '
                . 'title="' . t('Select from address book.') . '" onclick="load_address_select(' . $form['order_uid']['#value'] . ', \'#billing_address_select\', \'billing\');" '
                . 'style="position: relative; top: 2px; cursor: pointer;" />';
      $output .= ' <img src="' . base_path() . drupal_get_path('module', 'uc_store')
               . '/images/copy.gif" alt="' . t('Copy shipping information.') . '" title="'
               . t('Copy shipping information.') . '" onclick="uc_order_copy_shipping_to_billing();" '
               . 'style="position: relative; top: 2px; cursor: pointer;" />';
      return $output;

    case 'edit-theme':
      return drupal_render($form['bill_to']);

    case 'edit-process':
      foreach ($form_state['values'] as $key => $value) {
        if (substr($key, 0, 8) == 'billing_') {
          if (uc_address_field_enabled(substr($key, 8))) {
            $changes[$key] = $value;
          }
        }
      }
      return $changes;
  }
}

/**
 * Handles the "Customer Info" order pane.
 */
function uc_order_pane_customer($op, $order, &$form = NULL, &$form_state = NULL) {
  switch ($op) {
    case 'view':
      $build['uid'] = array('#markup' => t('Customer number: !user_link', array('!user_link' => $order->uid ? l($order->uid, 'user/' . $order->uid) : '0')));
      $build['primary_email'] = array('#markup' => '<br />' . t('Primary e-mail:') . '<br />' . check_plain($order->primary_email));

      return $build;

    case 'edit-form':
      $form['customer'] = array();

      $form['customer']['uid'] = array(
        '#type' => 'hidden',
        '#default_value' => $order->uid,
      );
      $form['customer']['text']['uid_text'] = array(
        '#type' => 'textfield',
        '#title' => t('Customer number'),
        '#default_value' => $order->uid,
        '#maxlength' => 10,
        '#size' => 10,
        '#disabled' => TRUE,
      );
      $form['customer']['primary_email'] = array(
        '#type' => 'hidden',
        '#default_value' => $order->primary_email,
      );
      $form['customer']['text']['primary_email_text'] = array(
        '#type' => 'textfield',
        '#title' => t('Primary e-mail'),
        '#default_value' => $order->primary_email,
        '#maxlength' => 64,
        '#size' => 32,
        '#disabled' => TRUE,
      );
      return $form;

    case 'edit-title':
      $output = ' <img src="' . base_path() . drupal_get_path('module', 'uc_store')
                . '/images/order_view.gif" alt="' . t('Search for an existing customer.') . '" '
                . 'title="' . t('Search for an existing customer.') . '" onclick="load_customer_search();" '
                . 'style="position: relative; top: 2px; cursor: pointer;" />';
      $output .= ' <img src="' . base_path() . drupal_get_path('module', 'uc_store')
                . '/images/menu_customers_small.gif" alt="' . t('Create a new customer.') . '" '
                . 'title="' . t('Create a new customer.') . '" onclick="load_new_customer_form();" '
                . 'style="position: relative; top: 2px; cursor: pointer;" />';
      return $output;

    case 'edit-theme':
      $output = '<div id="customer-select"></div><table class="order-edit-table">';
      foreach (element_children($form['customer']['text']) as $field) {
        $title = $form['customer']['text'][$field]['#title'];
        $form['customer']['text'][$field]['#title'] = NULL;
        $output .= '<tr><td class="oet-label">' . $title . ':</td><td>'
                 . drupal_render($form['customer']['text'][$field]) . '</td></tr>';
      }
      $output .= '</table>' . drupal_render($form['customer']['primary_email'])
               . drupal_render($form['customer']['uid']);
      return $output;

    case 'edit-process':
      $changes['uid'] = $form_state['values']['uid'];
      $changes['primary_email'] = $form_state['values']['primary_email'];
      return $changes;
  }
}

/**
 * Handles the "Products" order pane.
 */
function uc_order_pane_products($op, $order, &$form = NULL, &$form_state = NULL) {
  switch ($op) {
    case 'view':
      return tapir_get_table('uc_op_products_view_table', $order);

    case 'customer':
      return tapir_get_table('uc_op_products_customer_table', $order);

    case 'edit-form':
      $form['add_product_button'] = array(
        '#type' => 'submit',
        '#value' => t('Add product'),
        '#submit' => array('uc_order_pane_products_select'),
        '#ajax' => array(
          'callback' => 'uc_order_pane_products_ajax_callback',
          'wrapper' => 'product-controls',
        ),
      );
      $form['add_blank_line_button'] = array(
        '#type' => 'submit',
        '#value' => t('Add blank line'),
        '#submit' => array('uc_order_edit_products_add_blank'),
        '#ajax' => array(
          'callback' => 'uc_order_pane_products_ajax_callback',
          'wrapper' => 'product-controls',
        ),
      );

      $form['product_controls'] = array(
        '#tree' => TRUE,
        '#prefix' => '<div id="product-controls">',
        '#suffix' => '</div>',
      );

      $controls = array();

      if (isset($form_state['products_action'])) {
        switch ($form_state['products_action']) {
          case 'select':
            $controls = uc_order_product_select_form($form['product_controls'], $form_state, $order);
            break;
          case 'add_product':
            $controls = uc_order_add_product_form($form['product_controls'], $form_state, $order, $form_state['node']);
            break;
        }
      }

      $form['product_controls'] += $controls;

      $form += uc_order_edit_products_form($form, $form_state, $order->products);

      return $form;

    case 'edit-theme':
      $output = drupal_render($form['add_product_button']);
      $output .= drupal_render($form['add_blank_line_button']);
      $output .= drupal_render($form['product_controls']);
      $output .= drupal_render($form['products']);

      return $output;

    case 'edit-process':
      if (isset($form_state['values']['products'])) {
        foreach ($form_state['values']['products'] as $key => $product) {
          $product['data'] = unserialize($product['data']);
          uc_order_product_save($order->order_id, (object) $product);
        }
      }

      break;
  }
}

/**
 * Form to choose a product to add to the order.
 *
 * @ingroup forms
 */
function uc_order_product_select_form($form, &$form_state, $order) {
  $options = $form_state['product_select_options'];
  $ajax = array(
    'callback' => 'uc_order_pane_products_ajax_callback',
    'wrapper' => 'product-controls',
  );

  $form['nid'] = array(
    '#type' => 'select',
    '#title' => t('Select a product'),
    '#options' => $options,
    '#size' => 7,
    '#ajax' => $ajax + array(
      'event' => 'dblclick',
      'trigger_as' => array(
        'name' => 'op',
        'value' => t('Select'),
      ),
    ),
  );
  $form['product_search'] = array(
    '#type' => 'textfield',
    '#title' => t('Search by name or model/SKU (* is the wildcard)'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['select'] = array(
    '#type' => 'submit',
    '#value' => t('Select'),
    '#submit' => array('uc_order_pane_products_add'),
    '#ajax' => $ajax,
    '#weight' => 0,
  );
  $form['actions']['search'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
    '#submit' => array('uc_order_pane_products_select'),
    '#ajax' => $ajax,
    '#weight' => 1,
  );
  $form['actions']['close'] = array(
    '#type' => 'submit',
    '#value' => t('Close'),
    '#submit' => array('uc_order_pane_products_close'),
    '#ajax' => $ajax,
    '#weight' => 2,
  );

  return $form;
}

/**
 * Intermediate div that lets you set the qty and attributes for a product.
 *
 * @see uc_order_add_product_form()
 */
function uc_order_add_product($order, $product) {
  $build = array();
  $build['form'] = drupal_get_form('uc_order_add_product_form', $order, $product) + array(
    '#prefix' => '<div style="margin: 1em;"><strong>Add ' . check_plain($product->title) . '</strong>',
    '#suffix' => '</div>',
  );

  print drupal_render($build);
  exit();
}

/**
 * Sets the quantity and attributes of a product added to the order.
 *
 * @see uc_order_add_product_form()
 * @ingroup forms
 */
function uc_order_add_product_form($form, &$form_state, $order, $node) {
  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid,
  );
  $form['qty'] = array(
    '#type' => 'textfield',
    '#title' => t('Qty'),
    '#default_value' => '1',
    '#size' => 2,
    '#maxlength' => 5,
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add to order'),
    '#submit' => array('uc_order_edit_products_add'),
    '#ajax' =>  array(
      'callback' => 'uc_order_pane_products_ajax_callback',
      'wrapper' => 'product-controls',
    ),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#submit' => array('uc_order_pane_products_select'),
    '#ajax' =>  array(
      'callback' => 'uc_order_pane_products_ajax_callback',
      'wrapper' => 'product-controls',
    ),
  );
  $form['node'] = array(
    '#type' => 'value',
    '#value' => $node,
  );

  uc_form_alter($form, $form_state, __FUNCTION__);

  return $form;
}

/**
 * Form to allow ordered products' data to be changed.
 *
 * @see uc_op_products_edit_table()
 * @see theme_uc_order_edit_products_form()
 * @see theme_uc_order_remove_product()
 */
function uc_order_edit_products_form($form, &$form_state, $products) {
  if (($product_count = count($products)) > 0) {
    $form['products'] = tapir_get_table('uc_op_products_edit_table');
    for ($i = 0; $i < $product_count; $i++) {
      $form['products'][$i]['remove'] = array(
        '#type' => 'image_button',
        '#title' => t('Remove this product.'),
        '#src' => drupal_get_path('module', 'uc_store') . '/images/error.gif',
        '#attributes' => array(
          'style' => 'padding-top: 1px; padding-left: .8em; padding-right: 2px; float: left; cursor: pointer;',
        ),
        '#submit' => array('uc_order_edit_products_remove', 'uc_order_edit_form_submit'),
        '#return_value' => $products[$i]->order_product_id,
      );
      $form['products'][$i]['order_product_id'] = array(
        '#type' => 'hidden',
        '#value' => $products[$i]->order_product_id,
      );
      $form['products'][$i]['nid'] = array(
        '#type' => 'hidden',
        '#value' => $products[$i]->nid,
      );
      $form['products'][$i]['qty'] = array(
        '#type' => 'textfield',
        '#default_value' => $products[$i]->qty,
        '#size' => 2,
        '#maxlength' => 6,
      );
      $form['products'][$i]['title'] = array(
        '#type' => 'textfield',
        '#default_value' => $products[$i]->title,
        '#size' => 30,
      );
      $form['products'][$i]['model'] = array(
        '#type' => 'textfield',
        '#default_value' => $products[$i]->model,
        '#size' => 6,
      );
      $form['products'][$i]['weight'] = array(
        '#type' => 'textfield',
        '#default_value' => $products[$i]->weight,
        '#size' => 3,
      );
      $form['products'][$i]['cost'] = array(
        '#type' => 'textfield',
        '#default_value' => uc_store_format_price_field_value($products[$i]->cost),
        '#size' => 5,
      );
      $form['products'][$i]['price'] = array(
        '#type' => 'textfield',
        '#default_value' => uc_store_format_price_field_value($products[$i]->price),
        '#size' => 5,
      );
      $form['products'][$i]['data'] = array(
        '#type' => 'hidden',
        '#value' => serialize($products[$i]->data),
      );
    }
  }
  else {
    $form['products'][]['remove'] = array(
      '#value' => t('This order contains no products.'),
      '#cell_attributes' => array('colspan' => 'full'),
    );
  }

  return $form;
}

/**
 * @see uc_order_edit_products_form()
 * @ingroup themeable
 */
function theme_uc_order_edit_products_form($variables) {
  return drupal_render_children($variables['form']);
}

/**
 * Formats the button to remove products from an order.
 *
 * @see uc_op_products_edit_table()
 * @ingroup themeable
 */
function theme_uc_order_remove_product($variables) {
  $form = $variables['form'];

  return '<img id="' . $form['#return_value'] . '" src="'
    . $form['#src'] . '" style="padding-top: 1px; padding-left: .8em; '
    . 'padding-right: 2px; float: left; cursor: pointer;" '
    . 'alt="' . t('Remove this product.') . '" />'
    . drupal_render_children($form);
}

/**
 * Sets the order pane to show the product selection form.
 */
function uc_order_pane_products_select($form, &$form_state) {
  $types = uc_product_types();
  $options = array();

  if (!empty($form_state['values']['product_controls']['product_search'])) {
    $search = strtolower(str_replace('*', '%', $form_state['values']['product_controls']['product_search']));
    $search_args = array(
      ':types' => $types,
      ':title' => $search,
      ':model' => $search,
    );

    $result = db_query("SELECT n.nid, n.title FROM {node} AS n LEFT JOIN "
      . "{uc_products} AS p ON n.nid = p.nid WHERE n.type IN "
      . "(:types) AND (n.title LIKE :title OR p.model LIKE :model)"
      . " ORDER BY n.title", $search_args);
  }
  else {
    $result = db_query("SELECT nid, title FROM {node} WHERE type IN (:types) ORDER BY title", array(':types' => $types));
  }
  foreach ($result as $row) {
    $options[$row->nid] = $row->title;
  }

  if (count($options) == 0) {
    $options[0] = t('No products found.');
  }

  $form_state['products_action'] = 'select';
  $form_state['product_select_options'] = $options;
  unset($form_state['refresh_products']);
  $form_state['rebuild'] = TRUE;
}

/**
 * Sets the order pane to show the add product to order form.
 */
function uc_order_pane_products_add($form, &$form_state) {
  $form_state['products_action'] = 'add_product';
  $form_state['node'] = node_load($form_state['values']['product_controls']['nid']);
  unset($form_state['refresh_products']);
  $form_state['rebuild'] = TRUE;
}

/**
 * Hides the form to add another product to the order.
 */
function uc_order_pane_products_close($form, &$form_state) {
  unset($form_state['products_action']);
  unset($form_state['refresh_products']);
  unset($form_state['product_select_options']);
  $form_state['rebuild'] = TRUE;
}

/**
 * Populates the product add/edit div on the order edit screen.
 */
function uc_order_edit_products_add_blank($form, &$form_state) {
  $form_state['refresh_products'] = TRUE;
  $form_state['rebuild'] = TRUE;

  $order = $form_state['build_info']['args'][0];

  $product = new stdClass();
  $product->qty = 1;
  $product->order_id = $order->order_id;
  drupal_write_record('uc_order_products', $product);

  $order->products[] = $product;

  if (variable_get('uc_order_logging', TRUE)) {
    uc_order_log_changes($order->order_id, array('add' => t('Added new product line to order.')));
  }
}

/**
 *
 */
function uc_order_edit_products_add($form, &$form_state) {
  $form_state['products_action'] = 'products_select';
  $form_state['refresh_products'] = TRUE;
  $form_state['rebuild'] = TRUE;
  $order = $form_state['build_info']['args'][0];

  $product = node_load(intval($form_state['values']['product_controls']['nid']));
  $product->qty = isset($form_state['values']['product_controls']['qty']) ? $form_state['values']['product_controls']['qty'] : $product->default_qty;
  $product->price = $product->sell_price;
  $product->data = module_invoke_all('uc_add_to_cart_data', $form_state['values']['product_controls']);

  foreach (module_implements('uc_cart_item') as $module) {
    $function = $module . '_uc_cart_item';
    if (function_exists($function)) {
      // $product must be passed by reference.
      $function('load', $product);
    }
  }

  drupal_alter('uc_order_product', $product, $order);
  uc_order_product_save($order->order_id, $product);
  $order->products[] = $product;

  if (variable_get('uc_order_logging', TRUE)) {
    uc_order_log_changes($order->order_id, array('add' => t('Added (@qty) @title to order.', array('@qty' => $product->qty, '@title' => $product->title))));
  }

  // Decrement stock.
  if (module_exists('uc_stock')) {
    uc_stock_adjust_product_stock($product, 0, $order);
  }

  // Add this product to the form values for accurate tax calculations.
  $form_state['values']['products'][] = (array) $product;
}

/**
 *
 */
function uc_order_edit_products_remove($form, &$form_state) {
  $form_state['refresh_products'] = TRUE;

  $order_product_id = intval($form_state['triggering_element']['#return_value']);

  uc_order_product_delete($order_product_id);

  $order = $form_state['build_info']['args'][0];
  $matches = array();
  preg_match('/products\[(\d+)\]/', $form_state['triggering_element']['#name'], $matches);
  $key = $matches[1];

  unset($order->products[$key]);
  $order->products = array_values($order->products);
}

/**
 * AJAX callback to render the order product controls.
 */
function uc_order_pane_products_ajax_callback($form, &$form_state) {
  $commands[] = ajax_command_replace('#product-controls', drupal_render($form['product_controls']));
  $commands[] = ajax_command_prepend('#product-controls', theme('status_messages'));

  if (isset($form_state['refresh_products']) && $form_state['refresh_products']) {
    $commands[] = ajax_command_replace('#order-edit-products', drupal_render($form['products']));
    $commands[] = ajax_command_replace('#order-line-items', drupal_render($form['line_items']));
    $commands[] = ajax_command_prepend('#order-edit-products', theme('status_messages'));
  }

  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Handles the "Line Items" order pane.
 */
function uc_order_pane_line_items($op, $order, &$form = NULL, &$form_state = NULL) {
  switch ($op) {
    case 'view':
    case 'customer':
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

      $build['line_items'] = array(
        '#prefix' => '<table class="line-item-table">',
        '#suffix' => '</table>',
      );
      foreach ($line_items as $item) {
        $table_row = array(
          '#prefix' => '<tr>',
          '#suffix' => '</tr>',
        );

        $table_row['title'] = array(
          '#markup' => check_plain($item['title']),
          '#prefix' => '<td class="li-title">',
          '#suffix' => '</td>',
        );

        $table_row['amount'] = array(
          '#theme' => 'uc_price',
          '#price' => $item['amount'],
          '#prefix' => '<td class="li-amount">',
          '#suffix' => '</td>',
        );

        $build['line_items'][] = $table_row;
      }

      return $build;

    case 'show-title':
      return FALSE;

    case 'edit-form':
      $options = array();
      $items = _uc_line_item_list();
      $line_items = $order->line_items;
      foreach ($items as $item) {
        if (isset($item['add_list']) && $item['add_list'] === TRUE) {
          $options[$item['id']] = check_plain($item['title']);
        }
        if (isset($item['display_only']) && $item['display_only'] == TRUE) {
          $result = $item['callback']('display', $order);
          if (is_array($result)) {
            foreach ($result as $line) {
              $line_items[] = array(
                'line_item_id' => $line['id'],
                'title' => $line['title'],
                'amount' => $line['amount'],
                'weight' => $item['weight'],
              );
            }
          }
        }
      }
      usort($line_items, 'uc_weight_sort');

      $form['add_line_item'] = array();

      $form['add_line_item']['li_type_select'] = array(
        '#type' => 'select',
        '#title' => t('Select a type'),
        '#options' => $options,
      );
      $form['add_line_item']['actions'] = array('#type' => 'actions');
      $form['add_line_item']['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add line'),
        '#attributes' => array('class' => array('save-button')),
        '#disabled' => TRUE,
      );
      $form['li_delete_id'] = array(
        '#type' => 'hidden',
      );
      $form['line_items'] = array(
        '#tree' => TRUE,
        '#theme' => 'uc_order_pane_line_items',
        '#prefix' => '<div id="order-line-items">',
        '#suffix' => '</div>',
      );

      foreach ($line_items as $item) {
        $form['line_items'][$item['line_item_id']]['li_id'] = array(
          '#type' => 'hidden',
          '#value' => $item['line_item_id'],
        );
        if (isset($item['type']) && _uc_line_item_data($item['type'], 'stored') == TRUE) {
          $form['line_items'][$item['line_item_id']]['title'] = array(
            '#type' => 'textfield',
            '#title' => t('Title'),
            '#default_value' => $item['title'],
            '#size' => 40,
            '#maxlength' => 128,
            '#prefix' => '<img id="li-' . $item['line_item_id'] . '" src="'
                       . base_path() . drupal_get_path('module', 'uc_store')
                        . '/images/error.gif" alt="' . t('Remove line item.')
                        . '" title="' . t('Remove line item.') . '" style="'
                        . 'cursor: pointer; position: relative; top: 3px;" '
                        . 'onclick="confirm_line_item_delete(\''
                       . addslashes(t('Remove this line item?')) . '\', this.id);" />',
          );
          $form['line_items'][$item['line_item_id']]['amount'] = array(
            '#type' => 'textfield',
            '#title' => t('Amount'),
            '#default_value' => uc_store_format_price_field_value($item['amount']),
            '#size' => 6,
            '#maxlength' => 13,
            '#field_prefix' => variable_get('uc_sign_after_amount', FALSE) ? '' : variable_get('uc_currency_sign', '$'),
            '#field_suffix' => variable_get('uc_sign_after_amount', FALSE) ? variable_get('uc_currency_sign', '$') : '',
          );
        }
        else {
          $form['line_items'][$item['line_item_id']]['title'] = array(
            '#markup' => check_plain($item['title']),
          );
          $form['line_items'][$item['line_item_id']]['amount'] = array(
            '#theme' => 'uc_price',
            '#price' => $item['amount'],
          );
        }
      }
      return $form;

    case 'edit-theme':
      $form['add_line_item']['li_type_select']['#title'] = '';
      $output = '<table class="full-width"><tr><td>';
      $output .= '<table><tr><td colspan="2"><b>' . t('Add a line item')
                . ': </b></td></tr><tr><td>' . drupal_render($form['add_line_item']['li_type_select'])
                . ' </td><td>' . drupal_render($form['add_line_item']['actions'])
                . '</td></tr></table>';
      $output .= '</td><td>' . drupal_render($form['li_delete_id'])
                . drupal_render($form['line_items']) . '</td></tr></table>';
      return $output;

    case 'edit-process':
      uc_order_pane_line_items_submit($form, $form_state);
      return;

    case 'edit-ops':
      return array(t('Add line'));

    case t('Add line'):
      drupal_goto('admin/store/orders/' . $form_state['values']['order_id']
                 . '/add_line_item/' . $form_state['values']['li_type_select']);
  }
}

/**
 * @ingroup themeable
 */
function theme_uc_order_pane_line_items($variables) {
  $form = $variables['form'];

  $output = '<table class="line-item-table">';
  foreach (element_children($form) as $field) {
    $form[$field]['title']['#title'] = '';
    $form[$field]['amount']['#title'] = '';
    $output .= '<tr><td class="li-title">'
      . drupal_render($form[$field]['li_id'])
      . drupal_render($form[$field]['title'])
      . ':</td><td class="li-amount" nowrap>'
      . drupal_render($form[$field]['amount'])
      . '</td></tr>';
  }
  $output .= '</table>' . drupal_render_children($form);

  return $output;
}

/**
 *
 */
function uc_order_pane_line_items_submit($form, &$form_state) {
  $values = $form_state['values'];

  if (is_array($values['line_items'])) {
    foreach ($values['line_items'] as $line) {
      if (is_numeric($line['li_id']) && intval($line['li_id']) > 0) {
        uc_order_update_line_item($line['li_id'], $line['title'], $line['amount']);
      }
    }
  }
  if (intval($values['li_delete_id']) > 0) {
    uc_order_delete_line_item($values['li_delete_id']);
    drupal_set_message(t('Line item removed.'));
  }
}

/**
 *
 */
function uc_order_edit_line_items_recalculate($form, &$form_state) {
  $order = $form_state['build_info']['args'][0];

  // Load line items again, since some may have been updated by the form.
  $order->line_items = uc_order_load_line_items($order, TRUE);

  // Merge it with the defaultish line items.
  $order->line_items = array_merge($order->line_items, uc_order_load_line_items($order, FALSE));
  usort($order->line_items, 'uc_weight_sort');
}

/**
 * Handles the "Order Comments" order pane.
 */
function uc_order_pane_order_comments($op, $order, &$form = NULL, &$form_state = NULL) {
  switch ($op) {
    case 'view':
      $comments = uc_order_comments_load($order->order_id);
      return tapir_get_table('uc_op_order_comments_view_table', $comments);

    case 'customer':
      $comments = uc_order_comments_load($order->order_id);
      $header = array(t('Date'), t('Status'), array('data' => t('Message'), 'width' => '100%'));
      $rows[] = array(
        format_date($order->created, 'uc_store'),
        array('data' => '-', 'align' => 'center'),
        t('Order created.')
      );
      if (count($comments) > 0) {
        foreach ($comments as $comment) {
          $rows[] = array(
            'data' => array(
              array('data' => format_date($comment->created, 'uc_store'), 'align' => 'center'),
              array('data' => $comment->title, 'align' => 'center', 'nowrap' => 'nowrap'),
              check_plain($comment->message),
            ),
            'valign' => 'top'
          );
        }
      }
      $build = array(
        '#theme' => 'table',
        '#header' => $header,
        '#rows' => $rows,
      );

      return $build;
  }
}

/**
 * Handles the "Admin Comments" order pane.
 */
function uc_order_pane_admin_comments($op, $order, &$form = NULL, &$form_state = NULL) {
  switch ($op) {
    case 'view':
      $comments = uc_order_comments_load($order->order_id, TRUE);
      return tapir_get_table('uc_op_admin_comments_view_table', $comments);

    case 'edit-form':
      $form['admin_comment_field'] = array(
        '#type' => 'fieldset',
        '#title' => t('Add an admin comment'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form['admin_comment_field']['admin_comment'] = array(
        '#type' => 'textarea',
        '#description' => t('Admin comments are only seen by store administrators.'),
      );
      return $form;

    case 'edit-theme':
      $comments = uc_order_comments_load($form['order_id']['#value'], TRUE);
      if (is_array($comments) && count($comments) > 0) {
        foreach ($comments as $comment) {
          $items[] = '[' . uc_get_initials($comment->uid) . '] ' . filter_xss_admin($comment->message);
        }
      }
      else {
        $items = array(t('No admin comments have been entered for this order.'));
      }
      $output = theme('item_list', array('items' => $items)) . drupal_render($form['admin_comment_field']);
      return $output;

    case 'edit-process':
      if (!is_null($form_state['values']['admin_comment']) && strlen(trim($form_state['values']['admin_comment'])) > 0) {
        global $user;
        uc_order_comment_save($form_state['values']['order_id'], $user->uid, $form_state['values']['admin_comment']);
      }
      return;
  }
}

/**
 * Handles the "Update" order pane.
 */
function uc_order_pane_update($op, $order, &$form = NULL, &$form_state = NULL) {
  switch ($op) {
    case 'view':
      return drupal_get_form('uc_order_view_update_form', $order);
  }
}

/**
 * Form to save order comments and update the order status.
 *
 * @see theme_uc_order_view_update_controls()
 * @see uc_order_view_update_form_submit()
 * @ingroup forms
 */
function uc_order_view_update_form($form, &$form_state, $order) {
  $form['order_comment_field'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add an order comment'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['order_comment_field']['order_comment'] = array(
    '#type' => 'textarea',
    '#description' => t('Order comments are used primarily to communicate with the customer.'),
  );

  $form['admin_comment_field'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add an admin comment'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['admin_comment_field']['admin_comment'] = array(
    '#type' => 'textarea',
    '#description' => t('Admin comments are only seen by store administrators.'),
  );

  $form['current_status'] = array(
    '#type' => 'hidden',
    '#value' => $order->order_status,
  );

  $form['order_id'] = array(
    '#type' => 'hidden',
    '#value' => $order->order_id,
  );

  $form['controls'] = array(
    '#theme' => 'uc_order_view_update_controls',
    '#weight' => 10,
  );

  foreach (uc_order_status_list('general') as $status) {
    $options[$status['id']] = $status['title'];
  }
  foreach (uc_order_status_list('specific') as $status) {
    $options[$status['id']] = $status['title'];
  }
  $form['controls']['status'] = array(
    '#type' => 'select',
    '#title' => t('Order status'),
    '#default_value' => $order->order_status,
    '#options' => $options,
    '#attributes' => array('style' => 'display: inline;'),
    '#weight' => -10,
  );

  if (module_exists('rules')) {
    $form['controls']['notify'] = array(
      '#type' => 'checkbox',
      '#title' => t('Send e-mail notification on update.'),
    );
  }
  else {
    $form['controls']['notify'] = array(
      '#type' => 'value',
      '#value' => FALSE,
    );
  }

  $form['controls']['actions'] = array('#type' => 'actions');
  $form['controls']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#weight' => 10,
  );

  return $form;
}

/**
 * @see uc_order_view_update_form()
 * @ingroup themeable
 */
function theme_uc_order_view_update_controls($variables) {
  return '<div class="update-controls">' . drupal_render_children($variables['form']) . '</div>';
}

/**
 * Form submit handler for uc_order_view_update_form().
 *
 * @see uc_order_view_update_form()
 */
function uc_order_view_update_form_submit($form, &$form_state) {
  global $user;

  if ($form_state['values']['status'] != $form_state['values']['current_status']) {
    if (uc_order_update_status($form_state['values']['order_id'], $form_state['values']['status'])) {
      if (is_null($form_state['values']['order_comment']) || strlen($form_state['values']['order_comment']) == 0) {
        uc_order_comment_save($form_state['values']['order_id'], $user->uid, '-', 'order', $form_state['values']['status'], $form_state['values']['notify']);
      }
    }
  }

  if (!is_null($form_state['values']['order_comment']) && strlen(trim($form_state['values']['order_comment'])) > 0) {
    uc_order_comment_save($form_state['values']['order_id'], $user->uid, $form_state['values']['order_comment'], 'order', $form_state['values']['status'], $form_state['values']['notify']);
  }

  if (!is_null($form_state['values']['admin_comment']) && strlen(trim($form_state['values']['admin_comment'])) > 0) {
    uc_order_comment_save($form_state['values']['order_id'], $user->uid, $form_state['values']['admin_comment']);
  }

  // Let Rules send email if requested.
  if (module_exists('rules') && $form_state['values']['notify']) {
    $order = uc_order_load($form_state['values']['order_id']);
    rules_invoke_event('uc_order_status_email_update', $order);
  }

  drupal_set_message(t('Order updated.'));
}

/**
 * Builds the order view products table.
 */
function uc_op_products_view_table($order) {
  $table = array(
    '#type' => 'tapir_table',
    '#attributes' => array('class' => array('order-pane-table')),
  );

  $table['#columns']['qty'] = array(
    'cell' => array(
      'data' => t('Qty'),
      'class' => array('text-right'),
    ),
    'weight' => 0,
  );
  $table['#columns']['product'] = array(
    'cell' => array(
      'data' => t('Product'),
      'class' => array('header-wide'),
    ),
    'weight' => 1,
  );
  $table['#columns']['model'] = array(
    'cell' => array(
      'data' => t('SKU'),
      'class' => array('text-center'),
      'nowrap' => 'nowrap',
    ),
    'weight' => 2,
  );
  if (user_access('administer products')) {
    $table['#columns']['cost'] = array(
      'cell' => array(
        'data' => t('Cost'),
        'class' => array('text-right'),
      ),
      'weight' => 3,
    );
  }
  $table['#columns']['price'] = array(
    'cell' => array(
      'data' => t('Price'),
      'class' => array('text-right'),
    ),
    'weight' => 4,
  );
  $table['#columns']['total'] = array(
    'cell' => array(
      'data' => t('Total'),
      'class' => array('text-right'),
    ),
    'weight' => 5,
  );

  if (is_array($order->products)) {
    foreach ($order->products as $product) {
      $data = array();
      $data['qty'] = array(
        '#markup' => $product->qty . '&times;',
        '#cell_attributes' => array('align' => 'right'),
      );
      $data['product'] = array(
        '#markup' => check_plain($product->title) . uc_product_get_description($product),
      );
      $data['model'] = array(
        '#markup' => check_plain($product->model),
        '#cell_attributes' => array('align' => 'center'),
      );

      if (user_access('administer products')) {
        $data['cost'] = array(
          '#theme' => 'uc_price',
          '#price' => $product->cost,
          '#cell_attributes' => array('align' => 'right'),
        );
      }

      $data['price'] = array(
        '#theme' => 'uc_price',
        '#price' => $product->price,
        '#cell_attributes' => array('align' => 'right'),
      );
      $data['total'] = array(
        '#theme' => 'uc_price',
        '#price' => $product->price * $product->qty,
        '#cell_attributes' => array('align' => 'right'),
      );
      $data['#attributes'] = array('valign' => 'top');
      $table['#rows'][] = $data;
    }
  }
  else {
    $table['#rows'][]['product'] = array(
      '#markup' => t('This order contains no products.'),
      '#cell_attributes' => array('colspan' => 'full'),
    );
  }

  return $table;
}

/**
 * Builds the order customer's view products table.
 */
function uc_op_products_customer_table($order) {
  $table = array(
    '#type' => 'tapir_table',
    '#attributes' => array('class' => array('order-pane-table')),
  );

  $table['#columns']['qty'] = array(
    'cell' => array(
      'data' => t('Qty'),
      'class' => array('text-right'),
    ),
    'weight' => 0,
  );
  $table['#columns']['product'] = array(
    'cell' => array(
      'data' => t('Product'),
      'class' => array('header-wide'),
    ),
    'weight' => 1,
  );
  $table['#columns']['model'] = array(
    'cell' => array(
      'data' => t('SKU'),
      'class' => array('text-center'),
      'nowrap' => 'nowrap',
    ),
    'weight' => 2,
  );
  if (user_access('administer products')) {
    $table['#columns']['cost'] = array(
      'cell' => array(
        'data' => t('Cost'),
        'class' => array('text-right'),
      ),
      'weight' => 3,
    );
  }
  $table['#columns']['price'] = array(
    'cell' => array(
      'data' => t('Price'),
      'class' => array('text-right'),
    ),
    'weight' => 4,
  );
  $table['#columns']['total'] = array(
    'cell' => array(
      'data' => t('Total'),
      'class' => array('text-right'),
    ),
    'weight' => 5,
  );

  if (is_array($order->products)) {
    foreach ($order->products as $product) {
      $data = array();
      $data['qty'] = array(
        '#markup' => $product->qty . '&times;',
        '#cell_attributes' => array('align' => 'right'),
      );
      $data['product'] = array(
        '#markup' => check_plain($product->title) . uc_product_get_description($product),
      );
      $data['model'] = array(
        '#markup' => check_plain($product->model),
        '#cell_attributes' => array('align' => 'center'),
      );

      if (user_access('administer products')) {
        $data['cost'] = array(
          '#theme' => 'uc_price',
          '#price' => $product->cost,
          '#cell_attributes' => array('align' => 'right'),
        );
      }

      $data['price'] = array(
        '#theme' => 'uc_price',
        '#price' => $product->price,
        '#cell_attributes' => array('align' => 'right'),
      );
      $data['total'] = array(
        '#theme' => 'uc_price',
        '#price' => $product->price * $product->qty,
        '#cell_attributes' => array('align' => 'right'),
      );
      $data['#attributes'] = array('valign' => 'top');
      $table['#rows'][] = $data;
    }
  }
  else {
    $table['#rows'][]['product'] = array(
      '#markup' => t('This order contains no products.'),
      '#cell_attributes' => array('colspan' => 'full'),
    );
  }

  return $table;
}

/**
 * TAPIr table for products pane on the order edit page.
 *
 * @see uc_order_edit_products_form()
 */
function uc_op_products_edit_table() {
  $table = array(
    '#type' => 'tapir_table',
    '#tree' => TRUE,
    '#attributes' => array('id' => 'order-edit-products', 'class' => array('order-pane-table')),
  );

  $table['#columns']['remove'] = array(
    'cell' => t('Remove'),
    'weight' => 0,
  );
  $table['#columns']['qty'] = array(
    'cell' => t('Qty'),
    'weight' => 1,
  );
  $table['#columns']['title'] = array(
    'cell' => t('Name'),
    'weight' => 2,
  );
  $table['#columns']['model'] = array(
    'cell' => t('SKU'),
    'weight' => 4,
  );
  $table['#columns']['weight'] = array(
    'cell' => t('Weight'),
    'weight' => 5,
  );
  $table['#columns']['cost'] = array(
    'cell' => t('Cost'),
    'weight' => 6,
  );
  $table['#columns']['price'] = array(
    'cell' => t('Price'),
    'weight' => 7,
  );

  return $table;
}

/**
 * Builds the order comments table.
 */
function uc_op_order_comments_view_table($comments) {
  $table = array(
    '#type' => 'tapir_table',
    '#attributes' => array('class' => array('order-pane-table')),
  );

  $table['#columns']['date'] = array(
    'cell' => array('data' => t('Date'), 'class' => array('text-center')),
    'weight' => 0,
  );
  $table['#columns']['user'] = array(
    'cell' => t('User'),
    'weight' => 1,
  );
  $table['#columns']['notified'] = array(
    'cell' => t('Notified'),
    'weight' => 2,
  );
  $table['#columns']['status'] = array(
    'cell' => array('data' => t('Status'), 'class' => array('text-center')),
    'weight' => 3,
  );
  $table['#columns']['comment'] = array(
    'cell' => array('data' => t('Comment'), 'width' => '80%'),
    'weight' => 4,
  );

  if (is_array($comments) && !empty($comments)) {
    foreach ($comments as $comment) {
      $data = array();
      $data['date'] = array(
        '#markup' => format_date($comment->created, 'custom', variable_get('date_format_uc_store', 'm/d/Y') . '<\b\r />g:i:s A'),
        '#cell_attributes' => array('align' => 'center', 'nowrap' => 'true'),
      );
      $data['user'] = array(
        '#markup' => uc_get_initials($comment->uid),
        '#cell_attributes' => array('align' => 'center'),
      );
      $data['notified'] = array(
        '#markup' => (($comment->notified == 1)
          ? '<img src="' . base_path() . drupal_get_path('module', 'uc_order') . '/images/true-icon.gif" />'
          : '<img src="' . base_path() . drupal_get_path('module', 'uc_order') . '/images/false-icon.gif" />'),
        '#cell_attributes' => array('align' => 'center'),
      );
      $data['status'] = array(
        '#markup' => $comment->title,
        '#cell_attributes' => array('align' => 'center'),
      );
      $data['comment'] = array(
        '#markup' => check_plain($comment->message),
      );
      $table['#rows'][] = $data;
    }
  }
  else {
    $data['comment'] = array(
      '#markup' => t('This order has no comments associated with it.'),
      '#cell_attributes' => array('colspan' => 'full'),
    );
    $table['#rows'][] = $data;
  }

  return $table;
}

/**
 * Builds the order admin comments table.
 */
function uc_op_admin_comments_view_table($comments) {
  $table = array(
    '#type' => 'tapir_table',
    '#attributes' => array('class' => array('order-pane-table')),
  );

  $table['#columns']['date'] = array(
    'cell' => array('data' => t('Date'), 'class' => array('text-center')),
    'weight' => 0,
  );
  $table['#columns']['user'] = array(
    'cell' => array('data' => t('User'), 'class' => array('text-center')),
    'weight' => 1,
  );
  $table['#columns']['comment'] = array(
    'cell' => array('data' => t('Comment'), 'width' => '80%'),
    'weight' => 2,
  );

  if (is_array($comments) && !empty($comments)) {
    foreach ($comments as $comment) {
      $data = array();
      $data['date'] = array(
        '#markup' => format_date($comment->created, 'custom', variable_get('date_format_uc_store', 'm/d/Y') . '<\b\r />g:i:s A'),
        '#cell_attributes' => array('align' => 'center', 'nowrap' => 'true', 'valign' => 'top'),
      );
      $data['user'] = array(
        '#markup' => uc_get_initials($comment->uid),
        '#cell_attributes' => array('align' => 'center', 'valign' => 'top'),
      );
      $data['comment'] = array(
        '#markup' => filter_xss_admin($comment->message),
        '#cell_attributes' => array('valign' => 'top'),
      );
      $table['#rows'][] = $data;
    }
  }
  else {
    $data['comment'] = array(
      '#markup' => t('This order has no admin comments associated with it.'),
      '#cell_attributes' => array('colspan' => 'full'),
    );
    $table['#rows'][] = $data;
  }

  return $table;
}

/**
 * Builds a list of order panes defined in the enabled modules.
 */
function _uc_order_pane_list($view = 'view') {
  static $panes;

  if (count($panes) > 0) {
    return $panes;
  }

  $panes = module_invoke_all('uc_order_pane', NULL);
  foreach ($panes as $i => $value) {
    $panes[$i]['enabled'] = variable_get('uc_order_pane_' . $panes[$i]['id'] . '_enabled', (!isset($panes[$i]['enabled']) ? TRUE : $panes[$i]['enabled']));
    $panes[$i]['weight'] = variable_get('uc_order_pane_' . $panes[$i]['id'] . '_weight_' . $view, (!isset($panes[$i]['weight']) ? 0 : $panes[$i]['weight']));
  }
  usort($panes, 'uc_weight_sort');

  return $panes;
}

/**
 * Returns data from an order pane by pane ID and the array key.
 */
function _uc_order_pane_data($pane_id, $key) {
  $panes = _uc_order_pane_list();
  foreach ($panes as $pane) {
    if ($pane['id'] == $pane_id) {
      return $pane[$key];
    }
  }
}
