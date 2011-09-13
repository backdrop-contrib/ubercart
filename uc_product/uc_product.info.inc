<?php

/**
 * @file
 * Entity metadata hooks for uc_product.module.
 */

/**
 * Implements hook_entity_property_info_alter().
 */
function uc_product_entity_property_info_alter(&$info) {
  foreach (uc_product_types() as $type) {
    $properties = &$info['node']['bundles'][$type]['properties'];

    $properties['model'] = array(
      'label' => t('SKU'),
      'description' => t('The SKU or model number of the product.'),
      'type' => 'text',
    );
    $properties['list_price'] = array(
      'label' => t('List price'),
      'description' => t('The suggested retail price of the product.'),
      'type' => 'decimal',
    );
    $properties['cost'] = array(
      'label' => t('Cost'),
      'description' => t('The amount the store pays to sell the product.'),
      'type' => 'decimal',
    );
    $properties['sell_price'] = array(
      'label' => t('Sell price'),
      'description' => t('The amount the customer pays for the product.'),
      'type' => 'decimal',
    );
    $properties['weight'] = array(
      'label' => t('Weight'),
      'description' => t('The physical weight of the product.'),
      'type' => 'decimal',
    );
    $properties['weight_units'] = array(
      'label' => t('Weight units'),
      'description' => t('The unit of measure for the weight field.'),
      'type' => 'token',
    );
    $properties['length'] = array(
      'label' => t('Length'),
      'description' => t('The physical length of the product or its packaging.'),
      'type' => 'decimal',
    );
    $properties['width'] = array(
      'label' => t('Width'),
      'description' => t('The physical width of the product or its packaging.'),
      'type' => 'decimal',
    );
    $properties['height'] = array(
      'label' => t('Height'),
      'description' => t('The physical height of the product or its packaging.'),
      'type' => 'decimal',
    );
    $properties['length_units'] = array(
      'label' => t('Weight units'),
      'description' => t('The unit of measure for the length, width and height.'),
      'type' => 'token',
    );
    $properties['pkg_qty'] = array(
      'label' => t('Package quantity'),
      'description' => t('The number of products that fit in one package.'),
      'type' => 'integer',
    );
    $properties['ordering'] = array(
      'label' => t('Ordering'),
      'description' => t('The position of the product in product listings.'),
      'type' => 'integer',
    );
    $properties['shippable'] = array(
      'label' => t('Shippable'),
      'description' => t('Whether the product can be shipped or not.'),
      'type' => 'boolean',
    );
  }
}
