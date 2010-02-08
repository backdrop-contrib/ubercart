<?php
// $Id$

/**
 * @file
 * Catalog menu items.
 */

/**
 * Show the catalog page of the given category.
 */
function uc_catalog_browse($tid = 0) {
  return array(
    '#theme' => 'uc_catalog_browse',
    '#tid' => $tid,
  );
}

/**
 * Display a formatted catalog page.
 *
 * If the category has products in it, display them in a TAPIr table. Subcategories
 *   are linked along the top of the page. If it does not have products, display
 *   subcategories in a grid with their images and subcategories.
 *
 * @param $tid
 *   Catalog term id from URL.
 * @return
 *   Formatted HTML of the catalog page.
 * @ingroup themeable
 */
function theme_uc_catalog_browse($variables) {
  $tid = $variables['tid'];

  drupal_add_css(drupal_get_path('module', 'uc_catalog') .'/uc_catalog.css');

  $output = '';
  $catalog = uc_catalog_get_page((int)$tid);
  drupal_set_title(check_plain($catalog->name));
  drupal_set_breadcrumb(uc_catalog_set_breadcrumb($catalog->tid));
  $types = uc_product_types();
  $links = array();
  $child_list = array();
  foreach ($catalog->children as $child) {
    if (!empty($child->nodes)) {
      $links[] = array('title' => $child->name . (variable_get('uc_catalog_breadcrumb_nodecount', FALSE) ? ' ('. $child->nodes .')' : ''), 'href' => uc_catalog_path($child),
        'attributes' => array('rel' => 'tag'),
      );
    }
    if (!empty($child->image)) {
      $image = '<div>';
      if (module_exists('imagecache')) {
        $image .= l(theme('image_style', array(
            'style_name' => 'uc_category',
            'path' => $child->image['filepath'],
          )),
          uc_catalog_path($child), array('html' => TRUE));
      }
      else {
        $image .= l(theme('image', array(
            'path' => $child->image['filepath'],
          )),
          uc_catalog_path($child), array('html' => TRUE));
      }
      $image .= '</div>';
    }
    else {
      $image = '<div></div>';
    }
    $grandchildren = array();
    $j = 0;
    $max_gc_display = 3;
    foreach ($child->children as $i => $grandchild) {
      if ($j > $max_gc_display) {
        break;
      }
      $g_child_nodes = 0;
      foreach ($types as $type) {
        $g_child_nodes += taxonomy_term_count_nodes($grandchild->tid, $type);
      }
      if ($g_child_nodes) {
        $grandchildren[$i] = l($grandchild->name, uc_catalog_path($grandchild), array('class' => array('subcategory')));
        $j++;
      }
    }
    //$grandchildren = array_slice($grandchildren, 0, intval(count($grandchildren) / 2) + 1, TRUE);
    if ($j > $max_gc_display) {
      array_push($grandchildren, l(t('More...'), uc_catalog_path($child), array('class' => array('subcategory'))));
    }
    if ($child->nodes) {
      $cell_link = $image .'<strong>'. l($child->name, uc_catalog_path($child)) .'</strong>';
      if (variable_get('uc_catalog_show_subcategories', TRUE)) {
        $cell_link .= "<br/><span>". implode(', ', $grandchildren) ."</span>\n";
      }
      $child_list[] = $cell_link;
    }
  }
  if (!empty($catalog->image)) {
    if (module_exists('imagecache')) {
      $output .= theme('image_style', array(
        'style_name' => 'uc_thumbnail',
        'path' => $catalog->image['filepath'],
        'alt' => $catalog->name,
        'title' => $catalog->name,
        'attributes' => array('class' => array('category')),
      ));
    }
    else {
      $output .= theme('image', array(
        'path' => $catalog->image['filepath'],
        'alt' => $catalog->name,
        'title' => $catalog->name,
        'attributes' => array('class' => array('category')),
      ));
    }
  }

  $query = db_select('node', 'n')->extend('PagerDefault')->extend('TableSort')->addTag('node_access');
  $query->innerJoin('term_node', 'tn', 'n.vid = tn.vid');
  $query->innerJoin('uc_products', 'p', 'n.vid = p.vid');

  $query->condition('tn.tid', $catalog->tid)
    ->condition('n.status', 1)
    ->condition('n.type', $types, 'IN');

  $count_query = clone $query;
  $count_query->expression('COUNT(DISTINCT n.nid)');

  $query->setCountQuery($count_query);

  $query->fields('n', array(
      'nid',
      'title',
      'sticky',
      'created',
    ))
    ->fields('p', array(
      'model',
      'sell_price',
      'ordering',
    ))
    ->groupBy('nid')
    ->limit(variable_get('uc_product_nodes_per_page', 12));

  // Build an ORDER BY clause for the SELECT query based on table sort info.
  if (empty($_REQUEST['order'])) {
    $query->orderBy('ordering')
      ->orderBy('title')
      ->orderBy('nid');
  }
  else {
    $query->orderByHeader(uc_product_table_header());
  }

  // Get the selected nids.
  $catalog->products = $query->execute()->fetchCol();

  if (count($catalog->products)) {
    if (count($links)) {
      $output .= theme('links', array('links' => $links, 'attributes' => array('class' => array('links', 'inline', 'uc-categories')))) ."<br />\n";
    }
    $output .= $catalog->description;
    $output .= theme('uc_catalog_products', array('products' => $catalog->products));
    $output .= theme('pager');
  }
  else {
    // Display table of child categories similar to an osCommerce site's front page.
    $columns = variable_get('uc_catalog_category_columns', 3);
    $cat_rows = array();
    $row = array();
    $i = 1;
    foreach ($child_list as $cell) {
      $row[] = array('data' => $cell, 'class' => array('category'));
      if ($i % $columns == 0) {
        $cat_rows[] = $row;
        $row = array();
      }
      $i++;
    }
    if (count($row) > 0 && count($row) < $columns) {
      if (count($cat_rows) >= 1) {
        $row = array_merge($row, array_fill(count($row), $columns - count($row), array('data' => '&nbsp;', 'class' => array('category'))));
      }
      $cat_rows[] = $row;
    }
    $output .= $catalog->description;
    $output .= theme('table', array('rows' => $cat_rows, 'attributes' => array('class' => array('category'))));
  }

  return $output;
}

