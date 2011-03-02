<?php

/**
 * @file
 * Catalog administration menu items.
 */


/**
 * Displays an overview of the catalog settings.
 */
function uc_catalog_settings_overview() {
  // Theme all the pages beneath this path into summary overviews.
  return array(
    '#theme' => 'uc_summary_overview',
    '#summaries' => uc_summarize_child_form_pages('admin/store/settings/catalog/edit'),
  );
}

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
      '#prefix' => '<div>',
      '#summary' => t('The taxonomy vocabulary <a href="!edit-url">%name</a> is set as the product catalog.', array('!edit-url' => url('admin/structure/taxonomy/' . $catalog->machine_name), '%name' => $catalog->name)),
      '#suffix' => '</div>',
    );
    $form['catalog_vid']['#markup'] = $form['catalog_vid']['#summary'];
  }

  $vocabs = array();
  $vocabularies = taxonomy_vocabulary_load_multiple(FALSE);
  foreach ($vocabularies as $vid => $vocabulary) {
    $vocabs[$vid] = $vocabulary->name;
  }

  // JTR - catalog-top-level sub-textfield
  $form['catalog-top-level'] = array('#type' => 'fieldset',
    '#title' => t('Catalog top level'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#summary callback' => 'uc_summarize_form',
    '#attributes' => array('class' => array('catalog-top-level')),
  );
  $form['catalog-top-level']['uc_catalog_vid'] = array('#type' => 'select',
    '#title' => t('Catalog vocabulary'),
    '#description' => t("The taxonomy vocabulary that will be considered the product catalog."),
    '#default_value' => variable_get('uc_catalog_vid', 0),
    '#summary callback' => 'uc_summarize_null',
    '#options' => $vocabs,
  );
  $form['catalog-top-level']['uc_catalog_breadcrumb'] = array('#type' => 'checkbox',
    '#title' => t('Display the catalog breadcrumb'),
    '#summary callback' => 'uc_summarize_checkbox',
    '#summary arguments' => array(
      t('Catalog breadcrumb is being displayed.'),
      t('Catalog breadcrumb is hidden.'),
    ),
    '#default_value' => variable_get('uc_catalog_breadcrumb', TRUE),
  );
  $form['catalog-top-level']['uc_catalog_breadcrumb_nodecount'] = array('#type' => 'checkbox',
    '#title' => t('Display node counts in the catalog breadcrumb'),
    '#summary callback' => 'uc_summarize_checkbox',
    '#summary arguments' => array(
      t('Node count is being displayed in the catalog breadcrumb.'),
      t('Node count is not being displayed in the catalog breadcrumb.'),
    ),
    '#default_value' => variable_get('uc_catalog_breadcrumb_nodecount', FALSE),
  );
  $form['catalog-top-level']['uc_catalog_show_subcategories'] = array('#type' => 'checkbox',
    '#title' => t('Display subcategories in the catalog view'),
    '#summary callback' => 'uc_summarize_checkbox',
    '#summary arguments' => array(
      t('The catalog view is displaying subcategories.'),
      t('The catalog view is not displaying subcategories.'),
    ),
    '#default_value' => variable_get('uc_catalog_show_subcategories', TRUE),
  );
  $form['catalog-top-level']['uc_catalog_category_columns'] = array('#type' => 'select',
    '#title' => t('Number of columns in the grid of categories'),
    '#default_value' => variable_get('uc_catalog_category_columns', 3),
    '#summary' => t('Subcategories are being displayed in @columns columns.', array('@columns' => variable_get('uc_catalog_category_columns', 3))),
    '#options' => drupal_map_assoc(range(1, 5)),
  );

  $form['catalog-products-list'] = array('#type' => 'fieldset',
    '#title' => t('Catalog products list'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#summary callback' => 'uc_summarize_form',
    '#attributes' => array('class' => array('catalog-products-list')),
  );
  $form['catalog-products-list']['uc_product_nodes_per_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Product nodes per page'),
    '#summary' => t('There are @nodes product nodes displayed per page.', array('@nodes' => variable_get('uc_product_nodes_per_page', 12))),
    '#default_value' => variable_get('uc_product_nodes_per_page', 12),
    '#description' => t("Determines how many products will be listed on every catalog category. Notice that if you are using grid display it must be multiple of the grid width value, otherwise the last row will not match."),
    '#size' => 2,
  );

  return system_settings_form($form);
}

/**
 * Settings forms for using a product grid instead of a table in the catalog.
 *
 * @ingroup forms
 */
function uc_catalog_grid_settings_form($form, &$form_state) {
  $form['uc_catalog_grid_display'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display products in the catalog in a grid.'),
    '#summary callback' => 'uc_summarize_checkbox',
    '#summary arguments' => array(
      t('Products are displayed in a grid.'),
      t('Products are displayed in a table list.'),
    ),
    '#default_value' => variable_get('uc_catalog_grid_display', FALSE),
    '#description' => t('If unchecked, the default table display will be used and the settings on this form will be ignored.'),
  );
  $form['uc_catalog_grid_display_width'] = array(
    '#type' => 'select',
    '#title' => t('Number of columns in the product grid'),
    '#options' => drupal_map_assoc(range(1, 5)),
    '#default_value' => variable_get('uc_catalog_grid_display_width', 3),
    '#summary' => t('The grid will be displayed in @columns columns.', array('@columns' => variable_get('uc_catalog_grid_display_width', 3))),
  );

  $form['displayed_fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Displayed fields'),
    '#description' => t('The checked fields will be included in the product grid for each product in your catalog.'),
  );
  $form['displayed_fields']['uc_catalog_grid_display_title'] = array(
    '#type' => 'checkbox',
    '#title' => t('Product title'),
    '#default_value' => variable_get('uc_catalog_grid_display_title', TRUE),
  );
  $form['displayed_fields']['uc_catalog_grid_display_model'] = array(
    '#type' => 'checkbox',
    '#title' => t('Product SKU'),
    '#default_value' => variable_get('uc_catalog_grid_display_model', TRUE),
  );
  $form['displayed_fields']['uc_catalog_grid_display_image'] = array(
    '#type' => 'checkbox',
    '#title' => t('Default product image'),
    '#description' => t('The image will always be included in the product grid.'),
    '#default_value' => TRUE,
    '#disabled' => TRUE,
  );
  $form['displayed_fields']['uc_catalog_grid_display_sell_price'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sell price'),
    '#default_value' => variable_get('uc_catalog_grid_display_sell_price', TRUE),
  );
  $form['displayed_fields']['uc_catalog_grid_display_add_to_cart'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add to cart form'),
    '#default_value' => variable_get('uc_catalog_grid_display_add_to_cart', TRUE),
  );
  $form['displayed_fields']['uc_catalog_grid_display_attributes'] = array(
    '#type' => 'checkbox',
    '#title' => t('Attribute selection elements'),
    '#description' => t('Adds any available attribute selection form elements to the add to cart form if displayed in the grid.'),
    '#default_value' => variable_get('uc_catalog_grid_display_attributes', TRUE),
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
