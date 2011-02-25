<?php

/**
 * @file
 * Rules hooks for Ubercart attributes.
 */

/**
 * Implements hook_rules_condition_info().
 */
function uc_attribute_rules_condition_info() {
  $conditions = array();

  $conditions['uc_attribute_ordered_product_option'] = array(
    'label' => t('Order has a product with a particular attribute option'),
    'description' => t('Search the products of an order for a particular option.'),
    'group' => t('Order: Product'),
    'base' => 'uc_attribute_condition_ordered_product_option',
    'parameter' => array(
      'product' => array('type' => 'uc_order_product', 'label' => t('Product')),
      'option' => array('type' => 'integer', 'label' => t('Attribute option'), 'options list' => 'uc_attribute_condition_ordered_product_options_list'),
    ),
  );

  return $conditions;
}

/**
 * Returns TRUE if a product in the given order has the selected option.
 *
 * @see uc_attribute_condition_ordered_product_option_form()
 */
function uc_attribute_condition_ordered_product_option($order) {
  $result = FALSE;

  $match = unserialize($settings['attribute_option']);

  foreach ($order->products as $product) {
    if (!isset($product->data['attributes'])) {
      continue;
    }

    $attributes = $product->data['attributes'];

    // Once the order is made, the attribute data is changed to just the names.
    // If we can't find it by ID, check the names.
    if (is_int(key($attributes))) {
      if (in_array($settings['attribute_option'], $attributes)) {
        $result = TRUE;
        break;
      }
    }
    else {
      // Load the attribute data once, only if we need it.
      if (!isset($option)) {
        if ($option = uc_attribute_option_load($settings['attribute_option'])) {
          $attribute = uc_attribute_load($option->aid);
        }
      }

      if ($attribute) {
        if (isset($attributes[$attribute->name]) && $attributes[$attribute->name] == $option->name) {
          $result = TRUE;
          break;
        }
      }
    }
  }

  return $result;
}

/**
 * Options callback.
 *
 * @see uc_attribute_condition_ordered_product_option()
 */
function uc_attribute_condition_ordered_product_options_list() {
  $options = array();
  $result = db_query("SELECT a.aid, a.name AS attr_name, a.ordering, o.oid, o.name AS opt_name, o.ordering FROM {uc_attributes} AS a JOIN {uc_attribute_options} AS o ON a.aid = o.aid ORDER BY a.ordering, o.ordering");
  foreach ($result as $option) {
    $options[$option->attr_name][$option->oid] = $option->opt_name;
  }

  return $options;
}
