<?php
/**
 * @file
 *
 * Contains the controller for uc_cart_item entities.
 */
class UcCartItemController extends EntityAPIController {

  public function buildContent($item, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $content = array();
    $content += module_invoke($item->module, 'uc_cart_display', $item);
    if (!empty($content)) {
      $content['cart_item_id'] = array(
        '#type' => 'hidden',
        '#value' => $item->cart_item_id,
      );
    }
    return parent::buildContent($item, $view_mode, $langcode, $content);
  }

  public function attachLoad(&$items, $revision_id = FALSE) {
    foreach ($items as &$item) {
      $product = uc_product_load_variant($item->nid, $item->data);
      // Merge in fields from the product.
      foreach ($product as $key => $value) {
        $item->$key = $value;
      }
      $item->module = $item->data['module'];
    }
    parent::attachLoad($items, $revision_id);
  }
}
