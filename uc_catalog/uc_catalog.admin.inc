<?php

/**
 * @file
 * Catalog administration menu items.
 */


/**
 * Catalog settings form.
 *
 * Configures the display of the catalog breadcrumb.
 *
 * @ingroup forms
 */
function uc_catalog_settings_form($form, &$form_state) {
  $vid = variable_get('uc_catalog_vid', NULL);
  if ($vid) {
    $catalog = taxonomy_vocabulary_load($vid);

    $form['catalog_vid'] = array(
      '#markup' => '<p>' . t('The taxonomy vocabulary <a href="!edit-url">%name</a> is set as the product catalog.', array('!edit-url' => url('admin/structure/taxonomy/' . $catalog->machine_name), '%name' => $catalog->name)) . '</p>',
    );
  }

  $vocabs = array();
  $vocabularies = taxonomy_vocabulary_load_multiple(FALSE);
  foreach ($vocabularies as $vid => $vocabulary) {
    $vocabs[$vid] = $vocabulary->name;
  }

  $form['catalog-top-level']['uc_catalog_vid'] = array(
    '#type' => 'select',
    '#title' => t('Catalog vocabulary'),
    '#default_value' => variable_get('uc_catalog_vid', 0),
    '#options' => $vocabs,
  );
  $form['catalog-top-level']['uc_catalog_breadcrumb'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display the catalog breadcrumb'),
    '#default_value' => variable_get('uc_catalog_breadcrumb', TRUE),
  );

  return system_settings_form($form);
}

/**
 * Displays links to all products that have not been categorized.
 */
function uc_catalog_orphaned_products() {
  $build = array();

  if (variable_get('taxonomy_maintain_index_table', TRUE)) {
    $build['explanation'] = array(
      '#markup' => t('Orphaned products are products that you have created but not yet assigned to a category in your product catalog. All such products will appear as links below that you can follow to edit the product listings to assign them to categories.'),
      '#prefix' => '<p>',
      '#suffix' => '</p>',
    );

    $vid = variable_get('uc_catalog_vid', 0);
    $product_types = uc_product_types();
    $field = field_info_field('taxonomy_catalog');
    $types = array_intersect($product_types, $field['bundles']['node']);
    $result = db_query("SELECT DISTINCT n.nid, n.title FROM {node} AS n LEFT JOIN (SELECT ti.nid, td.vid FROM {taxonomy_index} AS ti LEFT JOIN {taxonomy_term_data} AS td ON ti.tid = td.tid WHERE td.vid = :vid) AS txnome ON n.nid = txnome.nid WHERE n.type IN (:types) AND txnome.vid IS NULL", array(':vid' => $vid, ':types' => $types));

    $rows = array();
    while ($node = $result->fetchObject()) {
      $rows[] = l($node->title, 'node/' . $node->nid . '/edit', array('query' => array('destination' => 'admin/store/products/orphans')));
    }

    if (count($rows) > 0) {
      $build['orphans'] = array(
        '#theme' => 'item_list',
        '#items' => $rows,
      );
    }
    else {
      $build['orphans'] = array(
        '#markup' => t('All products are currently listed in the catalog.'),
        '#prefix' => '<p>',
        '#suffix' => '</p>',
      );
    }
  }
  else {
    $build['orphans'] = array(
      '#markup' => t('The node terms index is not being maintained, so Ubercart can not determine which products are not entered into the catalog.'),
      '#prefix' => '<p>',
      '#suffix' => '</p>',
    );
  }

  return $build;
}
