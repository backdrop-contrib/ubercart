<?php

/**
 * @file
 * Views hooks.
 */

/**
 * Implements hook_views_data().
 */
function uc_cart_views_data() {
  //Make the uc_cart_products table visible for Views module
  $data['uc_cart_products']['table']['group'] = t('Cart product');
  $data['uc_cart_products']['table']['join']['node'] = array(
    'left_field' => 'nid',
    'field' => 'nid',
  );
  $data['uc_cart_products']['table']['join']['users'] = array(
    'left_field' => 'uid',
    'field' => 'cart_id',
  );
  $data['uc_cart_products']['nid'] = array(
    'title' => t('Cart product id'),
    'help' => t('The nid of the product for in a cart.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
      'name field' => 'title', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['uc_cart_products']['cart_id'] = array(
    'title' => t('Cart id'),
    'help' => t('The id of the cart (either user or session id).'),
    'field' => array(
      'handler' => 'uc_cart_handler_field_cart_user',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_user_uid',
      'name field' => 'name', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'cart_id',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['uc_cart_products']['qty'] = array(
    'title' => t('qty'),
    'help' => t('The amount ordered.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['uc_cart_products']['changed'] = array(
    'title' => t('Cart Product Line Changed Date'),
    'help' => t('The date the cart line was changed'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  $data['uc_cart_products']['ordering'] = array(
    'title' => t('List position'),
    'help' => t('The default sort criteria in the catalog.'),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}
