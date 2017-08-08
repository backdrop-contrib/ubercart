<?php
/**
 * @file
 *
 * Contains the controller for uc_cart_item entities.
 */
class UcCartItemController extends EntityDatabaseStorageController {
  /**
   * Implements EMWControllerInterface.
   */
  public function view($entities, $view_mode = 'full', $langcode = NULL, $page = NULL) {
    // For Field API and entity_prepare_view, the entities have to be keyed by
    // (numeric) id.
    $entities = entity_key_array_by_property($entities, $this->idKey);
    if (!empty($this->entityInfo['fieldable'])) {
      field_attach_prepare_view($this->entityType, $entities, $view_mode);
    }
    entity_prepare_view($this->entityType, $entities);
    $langcode = isset($langcode) ? $langcode : $GLOBALS['language_content']->langcode;

    $view = array();
    foreach ($entities as $entity) {
      $build = $this->buildContent($entity, $view_mode, $langcode);
      $build = array_merge($build, array(
        // If the entity type provides an implementation, use this instead the
        // generic one.
        // @see template_preprocess_entity()
        '#theme' => 'entity', // Todo: need a legitimate theme function.
        '#entity_type' => $this->entityType,
        '#entity' => $entity,
        '#view_mode' => $view_mode,
        '#language' => $langcode,
        '#page' => $page,
      ));
      // Allow modules to modify the structured entity.
      backdrop_alter(array($this->entityType . '_view', 'entity_view'), $build, $this->entityType);
      $key = isset($entity->{$this->idKey}) ? $entity->{$this->idKey} : NULL;
      $view[$this->entityType][$key] = $build;
    }
    return $view;
  }

  /**
   * Overrides EntityDatabaseStorageController::attachLoad().
   */
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

  /**
   * Saves a cart item entity.
   *
   * Cart items are deleted if saved with a quantity of zero.
   */
  public function save(EntityInterface $item) {
    $transaction = NULL;
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
  public function buildContent($product, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $content += module_invoke($product->data['module'], 'uc_cart_display', $product);
    if (!empty($content)) {
      $content['cart_item_id'] = array(
        '#type' => 'hidden',
        '#value' => isset($product->cart_item_id) ? $product->cart_item_id : 0,
      );
    }

    return $content;
  }
}


/**
 * UcCartItem extends Entity.
 *
 * A subclass of Entity is a requirement for creating a custom entity. It is
 * also a minimum requirement to define the methods included here, even as 
 * empty functions.
 */
class UcCartItem extends Entity {
  /**
   * The cart item ID.
   *
   * @var integer
   */
  public $cart_item_id;


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
   * Implements EntityInterface::bundle().
   */
  public function bundle() {}
  /**
   * Implements EntityInterface::label().
   */
  public function label() {}
  /**
   * Implements EntityInterface::uri().
   */
  public function uri() {}
}