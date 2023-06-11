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
  $config = config('uc_catalog.settings');
  $view = views_get_view('uc_catalog');
  $displays = array();
  foreach ($view->display as $display) {
    if ($display->display_plugin == 'page') {
      $displays[$display->id] = $display->display_title;
    }
  }

  $form['#config'] = 'uc_catalog.settings';
  $form['catalog-top-level']['uc_catalog_display'] = array(
    '#type' => 'select',
    '#title' => t('Catalog display'),
    '#default_value' => $config->get('uc_catalog_display'),
    '#options' => $displays,
  );

  $vocabulary = $config->get('uc_catalog_vocabulary');
  if ($vocabulary) {
    $catalog = taxonomy_vocabulary_load($vocabulary);

    $form['catalog_vocabulary'] = array(
      '#type' => 'help',
      '#markup' => t('The taxonomy vocabulary <a href="!edit-url">%name</a> is set as the product catalog.', array('!edit-url' => url('admin/structure/taxonomy/' . $catalog->machine_name), '%name' => $catalog->name)),
    );
  }

  $vocabs = array();
  $vocabularies = taxonomy_vocabulary_load_multiple(FALSE);
  foreach ($vocabularies as $name => $vocabulary) {
    $vocabs[$name] = $vocabulary->name;
  }

  $form['catalog-top-level']['uc_catalog_vocabulary'] = array(
    '#type' => 'select',
    '#title' => t('Catalog vocabulary'),
    '#default_value' => $config->get('uc_catalog_vocabulary'),
    '#options' => $vocabs,
  );
  $form['catalog-top-level']['uc_catalog_breadcrumb'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display the catalog breadcrumb'),
    '#default_value' => $config->get('uc_catalog_breadcrumb'),
  );

  return system_settings_form($form);
}

/**
 * Displays links to all published products that have not been categorized.
 * Restricted to published products because table taxonomy_index does not
 * contain data for unpublished nodes.
 */
function uc_catalog_orphaned_products() {
  $build = array();

  if (config_get('taxonomy.settings', 'maintain_index_table')) {
    $rows = array();
    foreach (_uc_catalog_orphaned_products() as $nid => $title) {
      $rows[] = l($title, 'node/' . $nid . '/edit', array('query' => array('destination' => 'admin/store/products/orphans')));
    }
    if (count($rows) > 0) {
      $build['orphans'] = array(
        '#theme' => 'item_list',
        '#items' => $rows,
      );
    }
    else {
      $build['orphans'] = array(
        '#type' => 'help',
        '#markup' => t('All products are currently listed in the catalog.'),
      );
    }
  }
  else {
    $build['orphans'] = array(
      '#type' => 'help',
      '#markup' => t('The node terms index is not being maintained, so Ubercart can not determine which products are not entered into the catalog.'),
    );
  }

  return $build;
}

/**
 * Returns a list of node titles keyed on nid of all published products that
 * do not have any catalog taxonomy terms. Restricted to published products
 * because table taxonomy_index does not contain data for unpublished nodes.
 */
function _uc_catalog_orphaned_products() {
  if (!config_get('taxonomy.settings', 'maintain_index_table')) {
    return array();
  }
  $vocabulary = config_get('uc_catalog.settings', 'uc_catalog_vocabulary');
  $product_types = uc_product_types();
  $field = field_info_field('taxonomy_catalog');
  $types = array_intersect($product_types, $field['bundles']['node']);
  return db_query('
    SELECT DISTINCT n.nid, n.title
    FROM {node} n
    LEFT JOIN (
      SELECT ti.nid, td.vocabulary
      FROM {taxonomy_index} ti
      LEFT JOIN {taxonomy_term_data} td ON ti.tid = td.tid
      WHERE td.vocabulary = :vocabulary
    ) txnome ON n.nid = txnome.nid
    WHERE n.type IN (:types)
    AND n.status = 1
    AND txnome.vocabulary IS NULL
    ', array(':vocabulary' => $vocabulary, ':types' => $types))
    ->fetchAllKeyed();
}

/**
 * Repairs the catalog taxonomy field if it is lost or deleted.
 */
function uc_catalog_repair_field() {
  foreach (uc_product_types() as $type) {
    uc_catalog_add_node_type($type);
  }
  uc_catalog_add_image_field();

  backdrop_set_message(t('The catalog taxonomy reference field has been repaired.'));
  backdrop_goto('admin/structure/types/manage/product/fields');
}
