<?php
/**
 * @file
 * Contains the controller for uc_cart_item entities.
 */

class UcCartItemController extends EntityPlusController {

  /**
   * Overrides EntityDatabaseStorageController::attachLoad().
   */
  public function attachLoad(&$items, $revision_id = FALSE) {
    foreach ($items as &$item) {
      $product = uc_product_load_variant($item->nid, $item->data);
      if ($product) {
        // Merge in fields from the product.
        foreach ($product as $key => $value) {
          $item->$key = $value;
        }
        // UcCartItem should not have a path (inherited from the node) so that
        // path module doesn't try to update the node's alias.
        unset($item->path);
      }
      $item->module = $item->data['module'];
    }
    parent::attachLoad($items, $revision_id);
  }

  /**
   * Saves a cart item entity.
   *
   * Cart items are deleted if saved with a quantity of zero.
   */
  public function save($item, $transaction = NULL) {
    if ($item->qty < 1) {
      if (isset($item->cart_item_id)) {
        parent::delete(array($item->cart_item_id), $transaction);
      }
    }
    else {
      $item->changed = REQUEST_TIME;
      parent::save($item, $transaction);
    }
  }

  /**
   * Overrides EntityAPIController::buildContent().
   */
  public function buildContent(EntityInterface $product, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $content += module_invoke($product->data['module'], 'uc_cart_display', $product);
    if (!empty($content)) {
      $content['cart_item_id'] = array(
        '#type' => 'hidden',
        '#value' => isset($product->cart_item_id) ? $product->cart_item_id : 0,
      );
    }

    return parent::buildContent($product, $view_mode, $langcode, $content);
  }
}

/**
 * Defines an item in the user's shopping cart and/or order.
 */
class UcCartItem extends Entity {
  /**
   * The cart item ID.
   *
   * @var integer
   */
  public $cart_item_id;

  function __construct(array $values = array()) {
    parent::__construct($values);
    unset($this->path);
  }

  /**
   * Implements EntityInterface::id().
   */
  public function id() {
    return isset($this->cart_item_id) ? $this->cart_item_id : NULL;
  }

  /**
   * Implements EntityInterface::entityType().
   */
  public function entityType() {
    return 'uc_cart_item';
  }

  /**
   * Implements EntityInterface::label().
   */
  public function label() {
    return t('Cart item @id', array('@id' => $this->cart_item_id));
  }

  /**
   * Implements EntityInterface::uri().
   */
  public function uri() {
    // Only used internally, so doesn't have a URI.
    return NULL;
  }
}
