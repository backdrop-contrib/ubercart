<?php
/**
 * @file
 * Plugin to provide access control/visibility based on whether the path is an
 * Ubercart path.
 *
 * @see PathLayoutAccess.
 */
class UcLayoutAccess extends LayoutAccess {
  /**
   * Constructor for a Layout access rule.
   */
  function __construct($plugin_name, array $data = array()) {
    parent::__construct($plugin_name, $data);
    $this->settings += array(
      'visibility_setting' => 1,
      'paths' => '',
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
    $base_path = $_GET['q'];
    $normal_path = backdrop_get_normal_path($base_path);
    $parts = explode('/', $normal_path);

    // Check whether the page is at a path known to be governed by Ubercart.
    $page_match = FALSE;
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

    // Check whether the page is one of the additional paths.
    $alias_path = backdrop_get_path_alias($base_path);
    $page_match |= backdrop_match_path($alias_path, $this->settings['paths']);

    // If there's a URL alias, we may still be at the un-aliased path so check
    // that as well.
    if ($alias_path !== $base_path) {
      $page_match |= backdrop_match_path($base_path, $this->settings['paths']);
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
    $form['help'] = array(
      '#type' => 'help',
      '#markup' => t('Default Ubercart pages are store admin pages, the cart, checkout, catalog, and product pages.'),
    );
    $form['paths'] = array(
      '#type' => 'textarea',
      '#title' => t('Additional paths'),
      '#default_value' => $this->settings['paths'],
      '#description' => t('Enter additional paths that should also be considered Ubercart pages, one path per line. The "*" character is a wildcard. Example paths are "node/1" for an individual piece of content or "node/*" for every piece of content. "@front" is the home page.', array('@front' => '<front>')),
      '#required' => TRUE,
    );
  }
}
