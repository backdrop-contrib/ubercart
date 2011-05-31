<?php

/**
 * @file
 * Defines page callbacks for the product module.
 */

/**
 * Returns an autocomplete list for product nodes.
 *
 * Using this autocomplete on a textfield will autocomplete based on product
 * titles or SKUs and leave the nid in the textfield.
 */
function uc_product_title_sku_autocomplete($string = '') {
  $matches = array();

  if ($string) {
    $string = db_like($string);

    $query = db_select('uc_products', 'p')->addTag('node_access')
      ->fields('n', array('nid', 'title'))
      ->fields('p', array('model', 'unique_hash'))
      ->condition('p.unique_hash', '', '<>')
      ->condition(db_or()
        ->condition('n.title', $string . '%', 'LIKE')
        ->condition('p.model', $string . '%', 'LIKE'))
      ->range(0, 10);
    $n = $query->leftJoin('node', 'n', 'n.nid = p.nid');
    $result = $query->execute();
    foreach ($result as $node) {
      $matches[$node->nid] = t('@title [@sku]', array('@title' => $node->title, '@sku' => $node->model));
    }
  }

  drupal_json_output($matches);
}
