<?php
/**
 * @file
 * Catalog administration menu items.
 */

/**
 * Implements hook_views_data_alter().
 */
function uc_catalog_views_data_alter(&$data) {
  $data['node']['term_node_tid_depth']['argument']['handler'] = 'uc_catalog_handler_argument_term_node_tid_depth_new';
}
