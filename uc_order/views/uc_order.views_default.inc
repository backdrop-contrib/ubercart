<?php

/**
 * @file
 * Provide default Views configurations for uc_order.module.
 */

/**
 * Implements hook_views_default_views().
 */
function uc_order_views_default_views() {
  $view = new view;
  $view->name = 'uc_orders';
  $view->description = '';
  $view->tag = 'Ubercart';
  $view->base_table = 'uc_orders';
  $view->human_name = '';
  $view->core = 0;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Orders';
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'view all orders';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '30';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'actions' => 'actions',
    'order_id' => 'order_id',
    'billing_full_name' => 'billing_full_name',
    'order_status' => 'order_status',
    'created' => 'created',
    'order_total' => 'order_total',
  );
  $handler->display->display_options['style_options']['default'] = 'order_id';
  $handler->display->display_options['style_options']['info'] = array(
    'actions' => array(
      'align' => '',
      'separator' => '',
    ),
    'order_id' => array(
      'sortable' => 1,
      'align' => '',
      'separator' => '',
    ),
    'billing_full_name' => array(
      'align' => '',
      'separator' => '',
    ),
    'order_status' => array(
      'sortable' => 1,
      'align' => '',
      'separator' => '',
    ),
    'created' => array(
      'sortable' => 1,
      'align' => '',
      'separator' => '',
    ),
    'order_total' => array(
      'sortable' => 1,
      'align' => '',
      'separator' => '',
    ),
  );
  $handler->display->display_options['style_options']['override'] = 1;
  $handler->display->display_options['style_options']['sticky'] = 1;
  $handler->display->display_options['style_options']['order'] = 'desc';
  /* Field: Order: Actions */
  $handler->display->display_options['fields']['actions']['id'] = 'actions';
  $handler->display->display_options['fields']['actions']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['actions']['field'] = 'actions';
  $handler->display->display_options['fields']['actions']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['actions']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['actions']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['actions']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['actions']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['actions']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['actions']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['actions']['alter']['html'] = 0;
  $handler->display->display_options['fields']['actions']['hide_empty'] = 0;
  $handler->display->display_options['fields']['actions']['empty_zero'] = 0;
  /* Field: Order: Order ID */
  $handler->display->display_options['fields']['order_id']['id'] = 'order_id';
  $handler->display->display_options['fields']['order_id']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['order_id']['field'] = 'order_id';
  $handler->display->display_options['fields']['order_id']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['order_id']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['order_id']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['order_id']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['order_id']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['order_id']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['order_id']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['order_id']['alter']['html'] = 0;
  $handler->display->display_options['fields']['order_id']['hide_empty'] = 0;
  $handler->display->display_options['fields']['order_id']['empty_zero'] = 0;
  $handler->display->display_options['fields']['order_id']['link_to_order'] = 1;
  /* Field: Order: Billing address: Full name */
  $handler->display->display_options['fields']['billing_full_name']['id'] = 'billing_full_name';
  $handler->display->display_options['fields']['billing_full_name']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['billing_full_name']['field'] = 'billing_full_name';
  $handler->display->display_options['fields']['billing_full_name']['label'] = 'Customer';
  $handler->display->display_options['fields']['billing_full_name']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['alter']['external'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['billing_full_name']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['billing_full_name']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['alter']['html'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['billing_full_name']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['billing_full_name']['hide_empty'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['empty_zero'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['hide_alter_empty'] = 0;
  /* Field: Order: Order total */
  $handler->display->display_options['fields']['order_total']['id'] = 'order_total';
  $handler->display->display_options['fields']['order_total']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['order_total']['field'] = 'order_total';
  $handler->display->display_options['fields']['order_total']['label'] = 'Total';
  $handler->display->display_options['fields']['order_total']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['order_total']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['order_total']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['order_total']['alter']['external'] = 0;
  $handler->display->display_options['fields']['order_total']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['order_total']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['order_total']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['order_total']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['order_total']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['order_total']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['order_total']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['order_total']['alter']['html'] = 0;
  $handler->display->display_options['fields']['order_total']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['order_total']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['order_total']['hide_empty'] = 0;
  $handler->display->display_options['fields']['order_total']['empty_zero'] = 0;
  $handler->display->display_options['fields']['order_total']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['order_total']['set_precision'] = 0;
  $handler->display->display_options['fields']['order_total']['precision'] = '0';
  $handler->display->display_options['fields']['order_total']['format_plural'] = 0;
  /* Field: Order: Creation date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['label'] = 'Purchase date';
  $handler->display->display_options['fields']['created']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['created']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['created']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['created']['alter']['external'] = 0;
  $handler->display->display_options['fields']['created']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['created']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['created']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['created']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['created']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['created']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['created']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['created']['alter']['html'] = 0;
  $handler->display->display_options['fields']['created']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['created']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['created']['hide_empty'] = 0;
  $handler->display->display_options['fields']['created']['empty_zero'] = 0;
  $handler->display->display_options['fields']['created']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['created']['date_format'] = 'uc_store';
  /* Field: Order: Order status */
  $handler->display->display_options['fields']['order_status']['id'] = 'order_status';
  $handler->display->display_options['fields']['order_status']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['order_status']['field'] = 'order_status';
  $handler->display->display_options['fields']['order_status']['label'] = 'Status';
  $handler->display->display_options['fields']['order_status']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['order_status']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['order_status']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['order_status']['alter']['external'] = 0;
  $handler->display->display_options['fields']['order_status']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['order_status']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['order_status']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['order_status']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['order_status']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['order_status']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['order_status']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['order_status']['alter']['html'] = 0;
  $handler->display->display_options['fields']['order_status']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['order_status']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['order_status']['hide_empty'] = 0;
  $handler->display->display_options['fields']['order_status']['empty_zero'] = 0;
  $handler->display->display_options['fields']['order_status']['hide_alter_empty'] = 0;
  /* Filter criterion: Order: Order ID */
  $handler->display->display_options['filters']['order_id']['id'] = 'order_id';
  $handler->display->display_options['filters']['order_id']['table'] = 'uc_orders';
  $handler->display->display_options['filters']['order_id']['field'] = 'order_id';
  $handler->display->display_options['filters']['order_id']['group'] = 0;
  $handler->display->display_options['filters']['order_id']['exposed'] = TRUE;
  $handler->display->display_options['filters']['order_id']['expose']['operator_id'] = 'order_id_op';
  $handler->display->display_options['filters']['order_id']['expose']['label'] = 'View order number';
  $handler->display->display_options['filters']['order_id']['expose']['operator'] = 'order_id_op';
  $handler->display->display_options['filters']['order_id']['expose']['identifier'] = 'order_id';
  /* Filter criterion: Order: Order status */
  $handler->display->display_options['filters']['order_status']['id'] = 'order_status';
  $handler->display->display_options['filters']['order_status']['table'] = 'uc_orders';
  $handler->display->display_options['filters']['order_status']['field'] = 'order_status';
  $handler->display->display_options['filters']['order_status']['value'] = array(
    '_active' => '_active',
  );
  $handler->display->display_options['filters']['order_status']['group'] = 0;
  $handler->display->display_options['filters']['order_status']['exposed'] = TRUE;
  $handler->display->display_options['filters']['order_status']['expose']['operator_id'] = 'order_status_op';
  $handler->display->display_options['filters']['order_status']['expose']['label'] = 'View by status';
  $handler->display->display_options['filters']['order_status']['expose']['operator'] = 'order_status_op';
  $handler->display->display_options['filters']['order_status']['expose']['identifier'] = 'order_status';
  $handler->display->display_options['filters']['order_status']['expose']['reduce'] = 0;

  /* Display: Admin page */
  $handler = $view->new_display('page', 'Admin page', 'admin_page');
  $handler->display->display_options['path'] = 'admin/store/orders/view';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'View orders';
  $handler->display->display_options['menu']['description'] = 'View and process the orders received through your website.';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['name'] = 'management';

  /* Display: Search */
  $handler = $view->new_display('page', 'Search', 'search');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Search orders';
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Order: Billing address: First name */
  $handler->display->display_options['filters']['billing_first_name']['id'] = 'billing_first_name';
  $handler->display->display_options['filters']['billing_first_name']['table'] = 'uc_orders';
  $handler->display->display_options['filters']['billing_first_name']['field'] = 'billing_first_name';
  $handler->display->display_options['filters']['billing_first_name']['operator'] = 'allwords';
  $handler->display->display_options['filters']['billing_first_name']['group'] = 0;
  $handler->display->display_options['filters']['billing_first_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['billing_first_name']['expose']['operator_id'] = 'billing_first_name_op';
  $handler->display->display_options['filters']['billing_first_name']['expose']['label'] = 'Billing first name';
  $handler->display->display_options['filters']['billing_first_name']['expose']['operator'] = 'billing_first_name_op';
  $handler->display->display_options['filters']['billing_first_name']['expose']['identifier'] = 'billing_first_name';
  $handler->display->display_options['filters']['billing_first_name']['expose']['required'] = 0;
  $handler->display->display_options['filters']['billing_first_name']['expose']['remember'] = 1;
  $handler->display->display_options['filters']['billing_first_name']['expose']['multiple'] = FALSE;
  /* Filter criterion: Order: Billing address: Last name */
  $handler->display->display_options['filters']['billing_last_name']['id'] = 'billing_last_name';
  $handler->display->display_options['filters']['billing_last_name']['table'] = 'uc_orders';
  $handler->display->display_options['filters']['billing_last_name']['field'] = 'billing_last_name';
  $handler->display->display_options['filters']['billing_last_name']['operator'] = 'allwords';
  $handler->display->display_options['filters']['billing_last_name']['group'] = 0;
  $handler->display->display_options['filters']['billing_last_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['billing_last_name']['expose']['operator_id'] = 'billing_last_name_op';
  $handler->display->display_options['filters']['billing_last_name']['expose']['label'] = 'Billing last name';
  $handler->display->display_options['filters']['billing_last_name']['expose']['operator'] = 'billing_last_name_op';
  $handler->display->display_options['filters']['billing_last_name']['expose']['identifier'] = 'billing_last_name';
  $handler->display->display_options['filters']['billing_last_name']['expose']['required'] = 0;
  $handler->display->display_options['filters']['billing_last_name']['expose']['remember'] = 1;
  $handler->display->display_options['filters']['billing_last_name']['expose']['multiple'] = FALSE;
  /* Filter criterion: Order: Billing address: Company */
  $handler->display->display_options['filters']['billing_company']['id'] = 'billing_company';
  $handler->display->display_options['filters']['billing_company']['table'] = 'uc_orders';
  $handler->display->display_options['filters']['billing_company']['field'] = 'billing_company';
  $handler->display->display_options['filters']['billing_company']['operator'] = 'allwords';
  $handler->display->display_options['filters']['billing_company']['group'] = 0;
  $handler->display->display_options['filters']['billing_company']['exposed'] = TRUE;
  $handler->display->display_options['filters']['billing_company']['expose']['operator_id'] = 'billing_company_op';
  $handler->display->display_options['filters']['billing_company']['expose']['label'] = 'Billing company';
  $handler->display->display_options['filters']['billing_company']['expose']['operator'] = 'billing_company_op';
  $handler->display->display_options['filters']['billing_company']['expose']['identifier'] = 'billing_company';
  $handler->display->display_options['filters']['billing_company']['expose']['required'] = 0;
  $handler->display->display_options['filters']['billing_company']['expose']['remember'] = 1;
  $handler->display->display_options['filters']['billing_company']['expose']['multiple'] = FALSE;
  /* Filter criterion: Order: Delivery address: First name */
  $handler->display->display_options['filters']['delivery_first_name']['id'] = 'delivery_first_name';
  $handler->display->display_options['filters']['delivery_first_name']['table'] = 'uc_orders';
  $handler->display->display_options['filters']['delivery_first_name']['field'] = 'delivery_first_name';
  $handler->display->display_options['filters']['delivery_first_name']['operator'] = 'allwords';
  $handler->display->display_options['filters']['delivery_first_name']['group'] = 0;
  $handler->display->display_options['filters']['delivery_first_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['delivery_first_name']['expose']['operator_id'] = 'delivery_first_name_op';
  $handler->display->display_options['filters']['delivery_first_name']['expose']['label'] = 'Delivery first name';
  $handler->display->display_options['filters']['delivery_first_name']['expose']['operator'] = 'delivery_first_name_op';
  $handler->display->display_options['filters']['delivery_first_name']['expose']['identifier'] = 'delivery_first_name';
  $handler->display->display_options['filters']['delivery_first_name']['expose']['required'] = 0;
  $handler->display->display_options['filters']['delivery_first_name']['expose']['remember'] = 1;
  $handler->display->display_options['filters']['delivery_first_name']['expose']['multiple'] = FALSE;
  /* Filter criterion: Order: Delivery address: Last name */
  $handler->display->display_options['filters']['delivery_last_name']['id'] = 'delivery_last_name';
  $handler->display->display_options['filters']['delivery_last_name']['table'] = 'uc_orders';
  $handler->display->display_options['filters']['delivery_last_name']['field'] = 'delivery_last_name';
  $handler->display->display_options['filters']['delivery_last_name']['operator'] = 'allwords';
  $handler->display->display_options['filters']['delivery_last_name']['group'] = 0;
  $handler->display->display_options['filters']['delivery_last_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['delivery_last_name']['expose']['operator_id'] = 'delivery_last_name_op';
  $handler->display->display_options['filters']['delivery_last_name']['expose']['label'] = 'Delivery last name';
  $handler->display->display_options['filters']['delivery_last_name']['expose']['operator'] = 'delivery_last_name_op';
  $handler->display->display_options['filters']['delivery_last_name']['expose']['identifier'] = 'delivery_last_name';
  $handler->display->display_options['filters']['delivery_last_name']['expose']['required'] = 0;
  $handler->display->display_options['filters']['delivery_last_name']['expose']['remember'] = 1;
  $handler->display->display_options['filters']['delivery_last_name']['expose']['multiple'] = FALSE;
  /* Filter criterion: Order: Delivery address: Company */
  $handler->display->display_options['filters']['delivery_company']['id'] = 'delivery_company';
  $handler->display->display_options['filters']['delivery_company']['table'] = 'uc_orders';
  $handler->display->display_options['filters']['delivery_company']['field'] = 'delivery_company';
  $handler->display->display_options['filters']['delivery_company']['operator'] = 'allwords';
  $handler->display->display_options['filters']['delivery_company']['group'] = 0;
  $handler->display->display_options['filters']['delivery_company']['exposed'] = TRUE;
  $handler->display->display_options['filters']['delivery_company']['expose']['operator_id'] = 'delivery_company_op';
  $handler->display->display_options['filters']['delivery_company']['expose']['label'] = 'Delivery company';
  $handler->display->display_options['filters']['delivery_company']['expose']['operator'] = 'delivery_company_op';
  $handler->display->display_options['filters']['delivery_company']['expose']['identifier'] = 'delivery_company';
  $handler->display->display_options['filters']['delivery_company']['expose']['required'] = 0;
  $handler->display->display_options['filters']['delivery_company']['expose']['remember'] = 1;
  $handler->display->display_options['filters']['delivery_company']['expose']['multiple'] = FALSE;
  if (module_exists('date_views')) {
    /* Filter criterion: Order: Creation date */
    $handler->display->display_options['filters']['created']['id'] = 'created';
    $handler->display->display_options['filters']['created']['table'] = 'uc_orders';
    $handler->display->display_options['filters']['created']['field'] = 'created';
    $handler->display->display_options['filters']['created']['operator'] = 'between';
    $handler->display->display_options['filters']['created']['group'] = 0;
    $handler->display->display_options['filters']['created']['exposed'] = TRUE;
    $handler->display->display_options['filters']['created']['expose']['operator_id'] = 'created_op';
    $handler->display->display_options['filters']['created']['expose']['operator'] = 'created_op';
    $handler->display->display_options['filters']['created']['expose']['identifier'] = 'created';
    $handler->display->display_options['filters']['created']['expose']['multiple'] = FALSE;
  }
  /* Filter criterion: Order: Order status */
  $handler->display->display_options['filters']['order_status']['id'] = 'order_status';
  $handler->display->display_options['filters']['order_status']['table'] = 'uc_orders';
  $handler->display->display_options['filters']['order_status']['field'] = 'order_status';
  $handler->display->display_options['filters']['order_status']['value'] = array(
    '_active' => '_active',
  );
  $handler->display->display_options['filters']['order_status']['group'] = 0;
  $handler->display->display_options['filters']['order_status']['exposed'] = TRUE;
  $handler->display->display_options['filters']['order_status']['expose']['operator_id'] = 'order_status_op';
  $handler->display->display_options['filters']['order_status']['expose']['label'] = 'View by status';
  $handler->display->display_options['filters']['order_status']['expose']['operator'] = 'order_status_op';
  $handler->display->display_options['filters']['order_status']['expose']['identifier'] = 'order_status';
  $handler->display->display_options['filters']['order_status']['expose']['reduce'] = 0;
  $handler->display->display_options['path'] = 'admin/store/orders/search';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Search orders';
  $handler->display->display_options['menu']['description'] = 'Search existing orders.';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'management';

  $views[$view->name] = $view;
  return $views;
}
