<?php

/**
 * @file
 * Token hooks.
 */

/**
 * Implements hook_token_info().
 */
function uc_product_token_info() {
  $product['model'] = array(
    'name' => t('Model number'),
    'description' => t("The product's model number."),
  );

  $product['list_price'] = array(
    'name' => t('List price'),
    'description' => t("The product's list price."),
  );
  $product['cost'] = array(
    'name' => t('Cost'),
    'description' => t("The product's cost."),
  );
  $product['sell_price'] = array(
    'name' => t('Sell price'),
    'description' => t("The product's sell price."),
  );

  $product['weight'] = array(
    'name' => t('Weight'),
    'description' => t("The product's formatted weight."),
  );

  $product['weight-value'] = array(
    'name' => t('Weight value'),
    'description' => t("The numerical value of the product's weight."),
  );
  $product['weight_units'] = array(
    'name' => t('Weight units'),
    'description' => t("The unit of measurement for the product's weight."),
  );

  $product['length'] = array(
    'name' => t('Length'),
    'description' => t("The product's formatted length."),
  );
  $product['width'] = array(
    'name' => t('Width'),
    'description' => t("The product's formatted width."),
  );
  $product['height'] = array(
    'name' => t('Height'),
    'description' => t("The product's formatted height."),
  );

  $product['length-value'] = array(
    'name' => t('Length value'),
    'description' => t("The numerical value of the product's length."),
  );
  $product['width-value'] = array(
    'name' => t('Width value'),
    'description' => t("The numerical value of the product's width."),
  );
  $product['height-value'] = array(
    'name' => t('Height value'),
    'description' => t("The numerical value of the product's height."),
  );
  $product['length_units'] = array(
    'name' => t('Length units'),
    'description' => t("The unit of measurement for the product's length, width, and height."),
  );

  return array(
    'tokens' => array('node' => $product),
  );
}

/**
 * Implements hook_tokens().
 */
function uc_product_tokens($type, $tokens, $data = array(), $options = array()) {
  $values = array();
  $sanitize = !empty($options['sanitize']);

  if ($type == 'node' && !empty($data['node']) && uc_product_is_product($data['node'])) {
    $node = $data['node'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'model':
          $values[$original] = $sanitize ? check_plain($node->model) : $node->model;
          break;

        case 'list_price':
          $values[$original] = $object->list_price;
          break;

        case 'cost':
          $values[$original] = $object->cost;
          break;

        case 'sell_price':
          $values[$original] = $object->sell_price;
          break;

        case 'weight':
          $values[$original] = uc_weight_format($object->weight, $object->weight_units);
          break;

        case 'weight-value':
          $values[$original] = $object->weight;
          break;

        case 'weight_units':
          $values[$original] = $object->weight_units;
          break;

        case 'length':
          $values[$original] = uc_length_format($object->length, $object->length_units);
          break;

        case 'width':
          $values[$original] = uc_length_format($object->width, $object->length_units);
          break;

        case 'height':
          $values[$original] = uc_length_format($object->height, $object->length_units);
          break;

        case 'length-raw':
          $values[$original] = $object->length;
          break;

        case 'width-raw':
          $values[$original] = $object->width;
          break;

        case 'height-raw':
          $values[$original] = $object->height;
          break;

        case 'length_units':
          $values[$original] = $object->length_units;
          break;
      }
    }

    return $values;
  }
}
