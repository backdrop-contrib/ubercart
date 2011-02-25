<?php

/**
 * @file
 * Entity Metadata hooks
 */

function uc_order_entity_property_info() {
  $address_info = uc_address_property_info();

  return array(
    'uc_order_product' => array(
      'properties' => array(
        'order_product_id' => array(
          'type' => 'integer',
          'label' => t('Order product ID'),
          'description' => t('The unique ID for the purchased product.'),
        ),
        'order_id' => array(
          'type' => 'integer',
          'label' => t('Order ID'),
          'description' => t('The order ID that owns the product.'),
          'required' => TRUE,
        ),
        'nid' => array(
          'type' => 'integer',
          'label' => t('Node ID'),
          'description' => t('The unique ID of the node representing the product.'),
          'clear' => array('node'),
        ),
        'node' => array(
          'type' => 'node',
          'label' => t('Node'),
          'description' => t('The node representing the product.'),
        ),
        'title' => array(
          'type' => 'text',
          'label' => t('Title'),
          'description' => t('The product title.'),
        ),
        'model' => array(
          'type' => 'text',
          'label' => t('Model/SKU'),
          'description' => t('The model number of the product.'),
        ),
        'qty' => array(
          'type' => 'integer',
          'label' => t('Quantity'),
          'description' => t('The number of the same product ordered.'),
        ),
        'cost' => array(
          'type' => 'decimal',
          'label' => t('Cost'),
          'description' => t('The cost to the store for the product.'),
        ),
        'price' => array(
          'type' => 'decimal',
          'label' => t('Price'),
          'description' => t('The price paid for the ordered product.'),
        ),
        'weight' => array(
          'type' => 'decimal',
          'label' => t('Weight'),
          'description' => t('The physical weight of the product.'),
        ),
      ),
    ),
    'uc_order' => array(
      'properties' => array(
        'order_id' => array(
          'type' => 'integer',
          'label' => t('Order ID'),
          'description' => t('Primary key: the order ID.'),
          'query callback' => 'entity_metadata_table_query',
        ),
        'uid' => array(
          'type' => 'integer',
          'label' => t('User ID'),
          'description' => t('The unique ID of the customer who placed the order.'),
          'clear' => array('customer'),
          'query callback' => 'entity_metadata_table_query',
          'setter permission' => 'edit orders',
        ),
        'customer' => array(
          'type' => 'user',
          'label' => t('Customer'),
          'destription' => t('The user who placed the order.'),
          'getter callback' => 'uc_order_uc_order_get_properties',
          'setter callback' => 'uc_order_uc_order_set_properties',
          'setter permission' => 'edit orders',
          'clear' => array('uid'),
        ),
        'delivery_address' => array(
          'type' => 'struct',
          'label' => t('Delivery address'),
          'description' => t('The destination of the shipped products.'),
          'getter callback' => 'uc_order_address_property_get',
          'setter callback' => 'uc_order_address_property_set',
          'setter permission' => 'edit orders',
          'property info' => $address_info,
        ),
        'billing_address' => array(
          'type' => 'struct',
          'label' => t('Billing address'),
          'description' => t('The destination of the bill and invoice.'),
          'getter callback' => 'uc_order_address_property_get',
          'setter callback' => 'uc_order_address_property_set',
          'setter permission' => 'edit orders',
          'property info' => $address_info,
        ),
        'order_status' => array(
          'type' => 'text',
          'label' => t('Order status'),
          'description' => t('The status of the order.'),
          'required' => TRUE,
          'options list' => 'uc_order_status_list',
          'setter permission' => 'edit orders',
          'query callback' => 'entity_metadata_table_query',
        ),
        'order_total' => array(
          'type' => 'decimal',
          'label' => t('Order total'),
          'description' => t('The total amount due for the order.'),
          'getter callback' => 'uc_order_get_total',
          'query callback' => 'entity_metadata_table_query',
        ),
        'primary_email' => array(
          'type' => 'text',
          'label' => t('Primary email'),
          'description' => t('The primary email address of the customer.'),
          'setter permission' => 'edit orders',
          'query callback' => 'entity_metadata_table_query',
        ),
        'payment_method' => array(
          'type' => 'text',
          'label' => t('Payment method'),
          'description' => t('The method of payment.'),
          'query callback' => 'entity_metadata_table_query',
        ),
        'created' => array(
          'type' => 'date',
          'label' => t('Date created'),
          'description' => t('The date the order was placed.'),
          'query callback' => 'entity_metadata_table_query',
        ),
        'modified' => array(
          'type' => 'date',
          'label' => t('Date modified'),
          'description' => t('The date the order was most recently changed.'),
          'query callback' => 'entity_metadata_table_query',
        ),
        'host' => array(
          'type' => 'text',
          'label' => t('Host IP'),
          'description' => t('Host IP address of the person paying for the order.'),
          'query callback' => 'entity_metadata_table_query',
        ),
        'products' => array(
          'type' => 'list<uc_order_product>',
          'label' => t('Products'),
          'description' => t('The products that have been ordered.'),
          'getter callback' => 'uc_order_get_properties',
          'setter callback' => 'uc_order_set_properties',
          'setter permission' => 'edit orders',
          'clear' => array('order_total'),
        ),
      ),
    ),
  );
}

/**
 * Callback for getting node properties.
 *
 * @see entity_metadata_node_entity_info_alter()
 */
function uc_order_uc_order_get_properties($order, array $options, $name, $entity_type) {
  switch ($name) {
    case 'customer':
      return $order->uid;
  }
}

/**
 * Callback for setting node properties.
 *
 * @see entity_metadata_node_entity_info_alter()
 */
function uc_order_uc_order_set_properties($order, $name, $value) {
  if ($name == 'customer') {
    $order->uid = $value;
  }
}
