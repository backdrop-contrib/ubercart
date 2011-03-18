<?php

/**
 * @file
 * Views handler: "Add to cart" form as a field.
 */

/**
 * Displays the "Add to cart" form like the product page.
 */
class uc_product_handler_field_addtocart extends views_handler_field {
  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    if (uc_product_is_product($values->{$this->aliases['type']})) {
      $type = node_type_get_type($values->{$this->aliases['type']});
      $base = $type->base;
      $product = node_load($values->{$this->aliases['nid']});
      if (function_exists('theme_' . $base . '_add_to_cart')) {
        return theme($base . '_add_to_cart', array('node' => $product));
      }
      elseif (function_exists('theme_uc_product_add_to_cart')) {
        return theme('uc_product_add_to_cart', array('node' => $product));
      }
    }
  }
}
