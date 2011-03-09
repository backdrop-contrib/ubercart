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
  $view->view_php = '';
  $view->base_table = 'uc_orders';
  $view->is_cacheable = FALSE;
  $view->api_version = '3.0-alpha1';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Orders';
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'view all orders';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
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
  $handler->display->display_options['fields']['actions']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['actions']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['actions']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['actions']['alter']['strip_tags'] = 0;
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
  $handler->display->display_options['fields']['order_id']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['order_id']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['order_id']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['order_id']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['order_id']['alter']['html'] = 0;
  $handler->display->display_options['fields']['order_id']['hide_empty'] = 0;
  $handler->display->display_options['fields']['order_id']['empty_zero'] = 0;
  $handler->display->display_options['fields']['order_id']['link_to_order'] = 1;
  /* Field: Order: Payer's full name */
  $handler->display->display_options['fields']['billing_full_name']['id'] = 'billing_full_name';
  $handler->display->display_options['fields']['billing_full_name']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['billing_full_name']['field'] = 'billing_full_name';
  $handler->display->display_options['fields']['billing_full_name']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['billing_full_name']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['billing_full_name']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['alter']['html'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['hide_empty'] = 0;
  $handler->display->display_options['fields']['billing_full_name']['empty_zero'] = 0;
  /* Field: Order: Order Status */
  $handler->display->display_options['fields']['order_status']['id'] = 'order_status';
  $handler->display->display_options['fields']['order_status']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['order_status']['field'] = 'order_status';
  $handler->display->display_options['fields']['order_status']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['order_status']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['order_status']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['order_status']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['order_status']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['order_status']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['order_status']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['order_status']['alter']['html'] = 0;
  $handler->display->display_options['fields']['order_status']['hide_empty'] = 0;
  $handler->display->display_options['fields']['order_status']['empty_zero'] = 0;
  /* Field: Order: Creation Date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['created']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['created']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['created']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['created']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['created']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['created']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['created']['alter']['html'] = 0;
  $handler->display->display_options['fields']['created']['hide_empty'] = 0;
  $handler->display->display_options['fields']['created']['empty_zero'] = 0;
  $handler->display->display_options['fields']['created']['date_format'] = 'time ago';
  /* Field: Order: Order Total */
  $handler->display->display_options['fields']['order_total']['id'] = 'order_total';
  $handler->display->display_options['fields']['order_total']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['order_total']['field'] = 'order_total';
  $handler->display->display_options['fields']['order_total']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['order_total']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['order_total']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['order_total']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['order_total']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['order_total']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['order_total']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['order_total']['alter']['html'] = 0;
  $handler->display->display_options['fields']['order_total']['hide_empty'] = 0;
  $handler->display->display_options['fields']['order_total']['empty_zero'] = 0;
  $handler->display->display_options['fields']['order_total']['set_precision'] = 0;
  $handler->display->display_options['fields']['order_total']['precision'] = '0';
  $handler->display->display_options['fields']['order_total']['format_plural'] = 0;
  /* Filter: Order: Order Status */
  $handler->display->display_options['filters']['order_status']['id'] = 'order_status';
  $handler->display->display_options['filters']['order_status']['table'] = 'uc_orders';
  $handler->display->display_options['filters']['order_status']['field'] = 'order_status';
  $handler->display->display_options['filters']['order_status']['exposed'] = TRUE;
  $handler->display->display_options['filters']['order_status']['expose']['operator'] = 'order_status_op';
  $handler->display->display_options['filters']['order_status']['expose']['label'] = 'View by status';
  $handler->display->display_options['filters']['order_status']['expose']['identifier'] = 'order_status';
  $handler->display->display_options['filters']['order_status']['expose']['reduce'] = 0;
  /* Filter: Order: Order ID */
  $handler->display->display_options['filters']['order_id']['id'] = 'order_id';
  $handler->display->display_options['filters']['order_id']['table'] = 'uc_orders';
  $handler->display->display_options['filters']['order_id']['field'] = 'order_id';
  $handler->display->display_options['filters']['order_id']['exposed'] = TRUE;
  $handler->display->display_options['filters']['order_id']['expose']['operator'] = 'order_id_op';
  $handler->display->display_options['filters']['order_id']['expose']['label'] = 'View order';
  $handler->display->display_options['filters']['order_id']['expose']['identifier'] = 'order_id';

  $views[$view->name] = $view;
  return $views;
}
