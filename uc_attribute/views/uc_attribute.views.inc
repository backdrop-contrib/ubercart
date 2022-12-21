<?php

/**
 * @file
 * Views 2 hooks and callback registries.
 */

/**
 * Implements hook_views_data().
 */
function uc_attribute_views_data() {
  $data = array();

  // Create a field for each option value for each product, they could be null.
  $result = db_query("SELECT aid, oid FROM {uc_attribute_options}");
  foreach ($result as $row) {
    $attr = uc_attribute_load($row->aid);
    $option = uc_attribute_option_load($row->oid);
    $data['uc_product_options_' . $row->aid . '_' . $row->oid]['table']['group'] = t('Product attributes');
    $data['uc_product_options_' . $row->aid . '_' . $row->oid]['table']['join']['node'] = array(
      'table' => 'uc_product_options',
      'left_field' => 'nid',
      'field' => 'nid',
      'extra' => array(
        array(
          'field' => 'oid',
          'value' => $row->oid,
        ),
      ),
    );
    $data['uc_product_options_' . $row->aid . '_' . $row->oid]['table']['group'] = t('Product attributes');
    $data['uc_product_options_' . $row->aid . '_' . $row->oid]['price'] = array(
      'title' => t('Price for !aid/!oid', array('!aid' => $attr->label ? $attr->label : $attr->name, '!oid' => $option->name)),
      'help' => 'Price of the product with the selected option.',
      'field' => array(
        'handler' => 'uc_product_handler_field_price',
        'click sortable' => TRUE,
        'float' => TRUE,
      ),
    );
    $data['uc_product_options_' . $row->aid . '_' . $row->oid]['cost'] = array(
      'title' => t('Cost for !aid/!oid', array('!aid' => $attr->label ? $attr->label : $attr->name, '!oid' => $option->name)),
      'help' => 'Cost of the product with the selected option.',
      'field' => array(
        'handler' => 'uc_product_handler_field_price',
        'click sortable' => TRUE,
        'float' => TRUE,
      ),
    );
    $data['uc_product_options_' . $row->aid . '_' . $row->oid]['weight'] = array(
      'title' => t('Weight for !aid/!oid', array('!aid' => $attr->label ? $attr->label : $attr->name, '!oid' => $option->name)),
      'help' => 'Weight of the product with the selected option.',
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
    );
  }

  // Create a filter for each product attribute.
  $result = db_query("SELECT aid, name, description FROM {uc_attributes}");
  foreach ($result as $row) {

    $data['uc_order_products']['attr_' . $row->aid] = array(
      'title' => $row->name,
      'group' => t('Ordered product: Attributes'),
      'help' => 'Filter ordered products by this attribute.',
      'filter' => array(
        'handler' => 'UcAttributeHandlerFilterAttr',
      ),
      'aid' => $row->aid,
    );

    // Create a filter for each product attribute.
    $data['uc_product_adjustments']['pattr_' . $row->aid] = array(
      'title' => $row->name,
      'help' => 'Filter by this attribute if it has its own SKU on the products\'s adjustments.',
      'filter' => array(
        'handler' => 'UcAttributeHandlerFilterProductAttr',
      ),
    );
  }

  $data['uc_product_adjustments']['table']['group'] = t('Product adjustments');

  $uc_product_adjustments = drupal_get_schema_unprocessed('uc_attribute', 'uc_product_adjustments');

  $data['uc_product_adjustments']['table']['join']['node'] = array(
    'left_field' => 'nid',
    'field' => 'nid',
  );

  $data['uc_product_adjustments']['model'] = array(
    'title' => t('Model'),
    'help' => $uc_product_adjustments['fields']['model']['description'],
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'relationship' => array(
      'base' => 'uc_product_stock',
      'base field' => 'sku',
      'relationship field' => 'model',
      'handler' => 'views_handler_relationship',
      'title' => t('Stock from adjustments'),
      'help' => t("Product's stock from the SKUs in the adjustments table."),
    ),
  );

  $data['uc_product_adjustments']['combination'] = array(
    'title' => t('Attributes'),
    'help' => t('Product combination of attributes.'),
    'field' => array(
      'handler' => 'UcAttributeHandlerFieldCombination',
    ),
  );

  $data['uc_product_adjustments']['combination_sell_price'] = array(
    'title' => t('Sell price w/attributes adjustment'),
    'help' => t('The sell price of the product with all the price adjustments from its combination of attributes.'),
    'field' => array(
      'additional fields' => array(
        'combination',
      ),
      'handler' => 'UcAttributeHandlerFieldCombinationPrice',
      'price' => 'sell_price',
      'float' => TRUE,
    ),
  );

  $data['uc_product_adjustments']['combination_cost_price'] = array(
    'title' => t('Cost price w/attributes adjustment'),
    'help' => t('The cost price of the product with all the price adjustments from its combination of attributes.'),
    'field' => array(
      'additional fields' => array(
        'combination',
      ),
      'handler' => 'UcAttributeHandlerFieldCombinationPrice',
      'price' => 'cost',
      'float' => TRUE,
    ),
  );

  return $data;
}

/**
 * Implements hook_views_data_alter().
 *
 * Additions to other Ubercart related module views' handlers.
 *
 * They are added here so that they don't take any preference in a new view
 * building process.
 */
function uc_attribute_views_data_alter(&$data) {

  $data['uc_order_products']['attributes'] = array(
    'title' => t('Attributes'),
    'help' => t('The attribute and options selected for this product.'),
    'group' => t('Ordered product: Attributes'),
    'field' => array(
      'table' => 'uc_order_products',
      'field' => 'data',
      'handler' => 'UcAttributeHandlerFieldOrderProductAttribute',
    ),
  );

  // Add stock relationship to the adjustments table.
  $data['uc_product_stock']['sku']['relationship'] = array(
    'base' => 'uc_product_adjustments',
    'base field' => 'model',
    'relationship field' => 'sku',
    'handler' => 'views_handler_relationship',
    'title' => t('Adjustments from stock'),
    'help' => t("Product's adjustments from the SKUs in the stock table."),
    'group' => t('Product adjustments'),
  );
}
