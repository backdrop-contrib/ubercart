<?php
// $Id$

/**
 * @file
 * Views hooks and callback registries.
 */

define('UC_PAYER_PREFIX', "Payer's");
define('UC_RECIPIENT_PREFIX', "Recipient's");

/**
 * Implementation of hook_views_data().
 */
function uc_order_views_data() {
  // Make the uc_orders table visible for Views module
  $data['uc_orders']['table']['group'] = t('Order');

  // Base and use of schema for descriptions added by sammys
  $order_schema = drupal_get_schema('uc_orders');
  $data['uc_orders']['table']['base'] = array(
    'field' => 'order_id',
    'title' => t('Orders'),
    'help' => t('Orders placed in your Ubercart store.'),
  );

  $data['users']['table']['join']['uc_orders'] = array(
    'left_field' => 'uid',
    'field' => 'uid',
  );

  // Optionally link to order and allow order_id as argument
  $data['uc_orders']['order_id'] = array(
    'title' => t('Order ID'),
    'help' => $order_schema['fields']['order_id']['description'],
    'field' => array(
      'handler' => 'uc_order_handler_field_order_id',
      'click sortable' => TRUE,
    ),
     'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    // Information for accepting an order id as an argument
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'title',
      'numeric' => TRUE,
      'validate type' => 'order_id',
    ),
  );

  $data['uc_order_statuses']['table']['join'] = array(
    'uc_orders' => array(
      'left_field' => 'order_status',
      'field' => 'order_status_id',
    ),
  );
  $data['uc_orders']['order_status'] = array(
    'title' => t('Order Status'),
    'help' => $order_schema['fields']['order_status']['description'],
    'field' => array(
      'handler' => 'uc_order_handler_field_order_status',
      'additional fields' => array(
        'title' => array('table' => 'uc_order_statuses', 'field' => 'title'),
      ),
      'click sortable' => TRUE,
    ),
     'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'uc_order_handler_filter_order_status',
    ),
  );

  $data['uc_orders']['uid'] = array(
    'title' => t('User ID'),
    'help' => $order_schema['fields']['uid']['description'],
    'relationship' => array(
      'base' => 'users',
      'field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('Order User'),
    ),
  );

  // Changed field handler to display as a price
  $data['uc_orders']['order_total'] = array(
    'title' => t('Order Total'),
    'help' => $order_schema['fields']['order_total']['description'],
    'field' => array(
      'handler' => 'uc_order_handler_field_money_amount',
      'click sortable' => TRUE,
      'float' => TRUE,
    ),
     'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  $data['uc_orders']['order_total_cost'] = array(
    'title' => t('Total cost'),
    'help' => t('The total cost of the products in the order.'),
    'field' => array(
      'handler' => 'uc_order_handler_field_order_cost',
      //'click sortable' => TRUE,
      'float' => TRUE,
    ),
  );

  $data['uc_orders']['payment_method'] = array(
    'title' => t('Payment Method'),
    'help' => $order_schema['fields']['payment_method']['description'],
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
     'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'uc_order_handler_filter_payment_method',
    ),
  );

  $data['uc_orders']['cc_type'] = array(
    'title' => t('Credit Card Type'),
    'help' => t('Credit card type used if the payment method was credit.'),
    'real field' => 'data',
    'field' => array(
      'handler' => 'uc_order_handler_field_order_cc_data',
      'cc field' => 'cc_type',
    ),
  );

  $data['uc_orders']['cc_number'] = array(
    'title' => t('Credit Card Number'),
    'help' => t('Credit card number (last 4) used if the payment method was credit.'),
    'real field' => 'data',
    'field' => array(
      'handler' => 'uc_order_handler_field_order_cc_data',
      'cc field' => 'cc_number',
    ),
  );

  $data['uc_orders']['created'] = array(
    'title' => t('Creation Date'),
    'help' => $order_schema['fields']['created']['description'],
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
  // Use the sleeker date_api views handler if the module is available
  if (module_exists('date_api')) {
    $data['uc_orders']['created']['filter']['handler'] = 'date_api_filter_handler';
  }

  $data['uc_orders']['product_count'] = array(
    'title' => t('Product count'),
    'help' => $order_schema['fields']['product_count']['description'] . " (currently only available for dev versions of UC)",
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

  $data['uc_orders']['modified'] = array(
    'title' => t('Modification Date'),
    'help' => $order_schema['fields']['modified']['description'],
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
  // Use the sleeker date_api views handler if the module is available
  if (module_exists('date_api')) {
    $data['uc_orders']['modified']['filter']['handler'] = 'date_api_filter_handler';
  }

  $data['uc_orders']['actions'] = array(
    'title' => t('Actions'),
    'help' => 'Returns the actions a user may perform on an order.',
    'field' => array(
      'handler' => 'uc_order_handler_field_order_actions',
      'click sortable' => FALSE,
    ),
  );

  $data['uc_orders']['primary_email'] = array(
    'title' => t('Email address'),
    'help' => $order_schema['fields']['primary_email']['description'],
    'field' => array(
      'handler' => 'views_handler_field_user_mail',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  $string_fields = array(
    'first_name' => 'first name',
    'last_name' => 'last name',
    'phone' => 'phone number',
    'company' => 'company',
    'street1' => 'address line 1',
    'street2' => 'address line 2',
    'city' => 'city',
    'postal_code' => 'zip/postal code',
  );

  foreach (array('delivery', 'billing') as $prefix) {
    $args = array(
      '!titleprefix' => ($prefix == 'delivery' ? UC_RECIPIENT_PREFIX : UC_PAYER_PREFIX),
    );
    foreach ($string_fields as $field => $suffix) {
      $data['uc_orders'][$prefix . '_' . $field] = array(
        'title' => t('!titleprefix !titlesuffix', $args + array('!titlesuffix' => $suffix)),
        'help' => $order_schema['fields'][$prefix . '_' . $field]['description'],
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE,
        ),
        'sort' => array(
          'handler' => 'views_handler_sort',
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string',
        ),
      );
    }

    $data['uc_orders'][$prefix . '_full_name'] = array(
      'title' => t('!titleprefix full name', $args),
      'help' => $prefix == 'delivery' ? t('The full name (first and last) of the person receiving shipment.') : t('The full name (first and last) of the person paying for the order.'),
      'field' => array(
        'additional fields' => array(
          $prefix . '_first_name',
          $prefix . '_last_name'
        ),
        'handler' => 'uc_order_handler_field_order_fullname',
        'prefix' => $prefix,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    );
  }

  $country_schema = drupal_get_schema('uc_countries');

  $country_views = array(
    'billing_countries' => array(
      'field' => 'billing_country',
      'prefix' => UC_PAYER_PREFIX,
    ),
    'delivery_countries' => array(
      'field' => 'delivery_country',
      'prefix' => UC_RECIPIENT_PREFIX,
    ),
  );

  foreach ($country_views as $key => $value) {
    $data['uc_orders'][$value['field']] = array(
      'title' => t('@prefix country', array('@prefix' => $value['prefix'])),
      'help' => t('Relate orders to @field', array('@field' => str_replace('_', ' ', $value['field']))),
      'filter' => array(
        'handler' => 'uc_order_handler_filter_country',
      ),
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'base' => 'uc_countries',
        'relationship table' => $key,
        'field' => 'country_id',
        'label' => t('@prefix country', array('@prefix' => $value['prefix'])),
      ),
    );

    $data[$key]['table']['group'] = t('Ubercart order');

    $data[$key]['table']['join']['uc_orders'] = array(
      'table' => 'uc_countries',
      'left_field' => $value['field'],
      'field' => 'country_id',
    );
    // Join to the uc_order_products, if used as base, via uc_orders
    $data[$key]['table']['join']['uc_order_products'] = $data[$key]['table']['join']['uc_orders'];
    $data[$key]['table']['join']['uc_order_products']['left_table'] = 'uc_orders';

    $data[$key]['country_id'] = array(
      'title' => t('@prefix country id', array('@prefix' => $value['prefix'])),
      'help' => $order_schema['fields'][$value['field']]['description'],
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
        'name field' => 'country_iso_code_2',
        'numeric' => TRUE,
        'validate type' => 'country_id',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );
    $data[$key]['country_name'] = array(
      'title' => t('@persons country name', array('@persons' => $value['prefix'])),
      'help' => str_replace(' ID ', ' ', $order_schema['fields'][$value['field']]['description']),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    );
    $data[$key]['country_iso_code_2'] = array(
      'title' => t('@persons country ISO code (2 characters)', array('@persons' => $value['prefix'])),
      'help' => $country_schema['fields']['country_iso_code_2']['description'],
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    );
    $data[$key]['country_iso_code_3'] = array(
      'title' => t('@persons country ISO code (3 characters)', array('@persons' => $value['prefix'])),
      'help' => $country_schema['fields']['country_iso_code_3']['description'],
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    );
  }

  $zone_schema = drupal_get_schema('uc_zones');

  $zone_views = array(
    'billing_zones' => array(
      'field' => 'billing_zone',
      'prefix' => UC_PAYER_PREFIX,
    ),
    'delivery_zones' => array(
      'field' => 'delivery_zone',
      'prefix' => UC_RECIPIENT_PREFIX,
    ),
  );

  foreach ($zone_views as $key => $value) {
    $data['uc_orders'][$value['field']] = array(
      'title' => t('@prefix zone', array('@prefix' => $value['prefix'])),
      'help' => t('Relate orders to @field.', array('@field' => str_replace('_', ' ', $value['field']))),
      'filter' => array(
        'handler' => 'uc_order_handler_filter_zone',
      ),
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'base' => 'uc_zones',
        'relationship table' => $key,
        'field' => 'zone_id',
        'label' => t('@persons zone', array('@persons' => $value['prefix'])),
      ),
    );

    $data[$key]['table']['group'] = t('Ubercart order');

    $data[$key]['table']['join']['uc_orders'] = array(
      'table' => 'uc_zones',
      'left_field' => $value['field'],
      'field' => 'zone_id',
    );

    // join to the uc_order_products, if used as base, via uc_orders
    $data[$key]['table']['join']['uc_order_products'] = $data[$key]['table']['join']['uc_orders'];
    $data[$key]['table']['join']['uc_order_products']['left_table'] = 'uc_orders';

    $data[$key]['zone_id'] = array(
      'title' => t('@persons zone id', array('@persons' => $value['prefix'])),
      'help' => $order_schema['fields'][$value['field']]['description'],
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      // Information for accepting an order id as an argument
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
        'name field' => 'zone_code',
        'numeric' => TRUE,
        'validate type' => 'zone_id',
      ),
      // Information for accepting an order id as a filter
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      // Information for sorting on an order id.
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );
    $data[$key]['zone_name'] = array(
      'title' => t('@persons zone name', array('@persons' => $value['prefix'])),
      'help' => preg_replace('/ id /i', ' ', $order_schema['fields'][$value['field']]['description']),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    );

    $data[$key]['zone_code'] = array(
      'title' => t('@persons zone code', array('@persons' => $value['prefix'])),
      'help' => str_replace('of the ', 'of the ' . strtolower($value['prefix']) . ' ', $zone_schema['fields']['zone_code']['description']),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    );
  }

  //Make the uc_order_products table visible for Views module
  $order_products = drupal_get_schema('uc_order_products');

  $data['uc_order_products']['table']['group'] = t('Order product');

  $data['uc_order_products']['table']['join']['uc_orders'] = array(
    'left_field' => 'order_id',
    'field' => 'order_id',
  );
  $data['uc_order_products']['table']['join']['node'] = array(
    'left_field' => 'nid',
    'field' => 'nid',
  );
  $data['node']['table']['join']['uc_order_products'] = array(
    'left_field' => 'nid',
    'field' => 'nid',
  );

  $data['uc_order_products']['model'] = array(
    'title' => t('SKU'),
    'help' => $order_products['fields']['model']['description'],
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['uc_order_products']['qty'] = array(
    'title' => t('Qty'),
    'help' => $order_products['fields']['qty']['description'],
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['uc_order_products']['price'] = array(
    'title' => t('Price'),
    'help' => $order_products['fields']['price']['description'],
    'field' => array(
      'handler' => 'uc_order_handler_field_money_amount',
      'click sortable' => TRUE,
      'float' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['uc_order_products']['cost'] = array(
    'title' => t('Cost'),
    'help' => $order_products['fields']['cost']['description'],
    'field' => array(
      'handler' => 'uc_order_handler_field_money_amount',
      'click sortable' => TRUE,
      'float' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['uc_order_products']['weight'] = array(
    'title' => t('Weight'),
    'help' => $order_products['fields']['weight']['description'],
    'field' => array(
      'additional fields' => array(
        'weight_units' => array(
          'table' => 'uc_products',
          'field' => 'weight_units',
        ),
      ),
      'handler' => 'uc_product_handler_field_weight',
      'click sortable' => TRUE,
      'float' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['uc_order_products']['nid'] = array(
    'title' => t('Product nid'),
    'help' => t('The Node of the ordered product'),
    'relationship' => array(
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'title' => t('Product node'),
    ),
  );
  $data['uc_order_products']['title'] = array(
    'title' => t('Product title'),
    'help' => $order_products['fields']['title']['description'],
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['uc_order_products']['data'] = array(
    'title' => t('Product attributes'),
    'help' => 'Filter by the value of a purchased products selected attributes.',
    'filter' => array(
      'handler' => 'views_handler_filter_in_operator',
    ),
  );

  $data['node']['table']['join']['uc_orders'] = array(
    'left_table' => 'uc_order_products',
    'left_field' => 'nid',
    'field' => 'nid',
  );

  $data['term_node']['table']['join'] = array(
    'uc_orders' => array(
      'left_table' => 'uc_order_products',
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  $data['uc_orders']['table']['join'] = array(
    'node' => array(
      'left_table' => 'uc_order_products',
      'left_field' => 'order_id',
      'field' => 'order_id',
    ),
    'uc_order_products' => array(
      'left_field' => 'order_id',
      'field' => 'order_id',
    ),
  );

  $comments_schema = drupal_get_schema('uc_order_comments');
  $data['uc_order_comments']['table']['group'] = t('Ubercart order comments');
  $data['uc_order_comments']['table']['join'] = array(
    'uc_orders' => array(
      'left_field' => 'order_id',
      'field' => 'order_id',
    ),
    'uc_order_products' => array(
      'left_table' => 'uc_orders',
      'left_field' => 'order_id',
      'field' => 'order_id',
    ),
  );

  $data['uc_order_comments']['message'] = array(
    'title' => t('Order Comments'),
    'help' => $comments_schema['fields']['message']['description'],
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
  );

  // Support for any module's line item, if new modules defines other line items
  // the views cache will have to be rebuilt
  // Although new line items views support should be defined on each module,
  // I don't think this wider apporach would harm. At most, it will duplicate
  // line items
  $line_items = array();
  foreach (_uc_line_item_list() as $line_item) {
    if (!in_array($line_item['id'], array('subtotal', 'tax_subtotal', 'total', 'generic')) && $line_item['stored']) {
      $line_items[$line_item['id']] = $line_item['title'];
    }
  }
  foreach ($line_items as $line_item_id => $line_item_desc) {
    $data['uc_order_line_items_' . $line_item_id]['table']['join']['uc_orders'] = array(
      'table' => 'uc_order_line_items',
      'left_field' => 'order_id',
      'field' => 'order_id',
      'extra' => array(
        array(
          'field' => 'type',
          'value' => $line_item_id,
        ),
      ),
    );
    $data['uc_order_line_items_' . $line_item_id]['table']['join']['uc_order_products'] = $data['uc_order_line_items_' . $line_item_id]['table']['join']['uc_orders'];
    $data['uc_order_line_items_' . $line_item_id]['table']['join']['uc_order_products']['left_table'] = 'uc_orders';

    $data['uc_order_line_items_' . $line_item_id]['table']['group'] = t('Ubercart order line item');
    $data['uc_order_line_items_' . $line_item_id]['title'] = array(
      'title' => t('!line_item_desc title', array('!line_item_desc' => $line_item_desc)),
      'help' => t('!line_item_desc order line item', array('!line_item_desc' => $line_item_desc)),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    );

    $data['uc_order_line_items_' . $line_item_id]['amount'] = array(
      'title' => t('!line_item_desc amount', array('!line_item_desc' => $line_item_desc)),
      'help' => t('!line_item_desc order line item', array('!line_item_desc' => $line_item_desc)),
      'field' => array(
        'handler' => 'uc_order_handler_field_money_amount',
        'click sortable' => TRUE,
        'float' => TRUE,
      ),
    );
  }

  return $data;
}

