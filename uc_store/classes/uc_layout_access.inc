<?php
/**
 * @file
 * Plugin to provide access control/visibility based on whether the path is an
 * Ubercart path.
 *
 * @see PathLayoutAccess.
 */
class UbercartLayoutAccess extends LayoutAccess {
  /**
   * Constructor for a Layout access rule.
   */
  function __construct($plugin_name, array $data = array()) {
    parent::__construct($plugin_name, $data);
    $this->settings += array(
      'visibility_setting' => 1,
    );
  }

  /**
   * {@inheritdoc}
   */
  function summary() {
    if ($this->settings['visibility_setting']) {
      return t('Current page is an Ubercart page.');
    }
    else {
      return t('Current page is not an Ubercart page.');
    }
  }

  /**
   * {@inheritdoc}
   */
  function checkAccess() {
    $path = backdrop_get_normal_path($_GET['q']);
    $parts = explode('/', $path);

    // Check whether the page is governed by Ubercart.
    $page_match == FALSE;
    switch ($parts[0]) {
      case 'admin':
        // No footer on /admin or /admin/*, but add a footer on /admin/store and
        // /admin/store/*.
        $page_match = isset($parts[1]) && $parts[1] == 'store';
        break;

      case 'node':
        // No footer on /node or /node/[type]/add. Only add a footer on
        // /node/[nid] if that node is a product.
        if (count($parts) == 2 && intval($parts[1]) != 0) {
          $node = node_load($parts[1]);
          $page_match = !empty($node) && uc_product_is_product($node->type);
        }

      case 'catalog':
      case 'cart':
        $page_match = TRUE;
        break;

      default:
    }

    // When $this->settings['visibility_setting'] has a value of 0, the block is
    // displayed on all pages except those listed. When set to 1, it is
    // displayed only on those pages listed.
    if (!$this->settings['visibility_setting']) {
      $page_match = !$page_match;
    }

    return $page_match;
  }

  /**
   * {@inheritdoc}
   */
  function form(&$form, &$form_state) {
    parent::form($form, $form_state);

    $form['visibility_setting'] = array(
      '#type' => 'radios',
      '#options' => array(
        1 => t('Allow access on all Ubercart pages'),
        0 => t('Allow access on all pages except Ubercart pages'),
      ),
      '#default_value' => $this->settings['visibility_setting'],
    );
  }
}
