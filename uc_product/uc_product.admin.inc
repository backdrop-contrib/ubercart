<?php

/**
 * @file
 * Product administration menu items.
 */

/**
 * Lists all products.
 */
function uc_product_administration() {
  $build = array();

  $header = array();
  $tbl_columns = uc_product_table_header();
  foreach ($tbl_columns as $column) {
    $header[] = $column['cell'];
  }

  // Display 50 products per page.
  $query = db_select('node', 'n')->extend('PagerDefault')->extend('TableSort')
    ->fields('n', array('nid'))
    ->orderByHeader($header)
    ->limit(50);
  $query->join('uc_products', 'p', 'n.vid = p.vid');

  $nids = $query->execute()->fetchCol();

  // Display the product table and pager if necessary.
  $build['table'] = tapir_get_table('uc_product_table', $nids);
  $build['pager'] = array(
    '#theme' => 'pager',
    '#weight' => 5,
  );

  return $build;
}

/**
 * Displays a list of product classes.
 */
function uc_product_class_default() {
  $result = db_query("SELECT * FROM {uc_product_classes}");
  $header = array(t('Class ID'), t('Name'), t('Description'), t('Operations'));
  $rows = array();
  foreach ($result as $class) {
    $ops = array(
      l(t('edit'), 'admin/store/products/classes/' . $class->pcid . '/edit'),
      l(t('delete'), 'admin/store/products/classes/' . $class->pcid . '/delete'),
    );
    $rows[] = array(
      $class->pcid,
      $class->name,
      $class->description,
      implode(' ', $ops),
    );
  }
  if (count($rows) == 0) {
    $rows[] = array(array('data' => t('No product classes have been defined yet.'), 'colspan' => '5'));
  }

  $build = array();
  $build['product_classes'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  $build['header'] = array(
    '#markup' => '<h2>' . t('Add a class') . '</h2>',
  );
  $build['form'] = drupal_get_form('uc_product_class_form');

  return $build;
}

/**
 * Displays overview of product settings.
 *
 * @see uc_product_settings_form()
 * @see uc_product_field_settings_form()
 */
function uc_product_settings_overview() {
  // Load the form summaries for pages beneath this path.
  $summaries = uc_summarize_child_form_pages('admin/store/settings/products/edit');

  // Theme it all up in a summaries overview.
  return array(
    '#theme' => 'uc_summary_overview',
    '#summaries' => $summaries,
  );
}

/**
 * Form to change product settings.
 *
 * @see uc_product_settings_overview()
 * @ingroup forms
 */
function uc_product_settings_form($form, &$form_state) {
  // Loop through all the integrated image widgets and build an options array.
  $options = array();

  foreach (module_invoke_all('uc_image_widget') as $key => $widget) {
    $options[$key] = check_plain($widget['name']);
  }

  if (empty($options)) {
    $options[NULL] = t('No image widgets installed.');
  }
  else {
    // If we have widgets installed, add option to not use any of them
    $options['none'] = "Don't use any image widgets.";
  }

  $form['uc_product_image_widget'] = array(
    '#type' => 'radios',
    '#title' => t('Product image widget'),
    '#description' => t('The selected widget will be used to display a zoomed version of product images when they are clicked.'),
    '#options' => $options,
    '#default_value' => variable_get('uc_product_image_widget', NULL),
  );

  $form['uc_product_add_to_cart_qty'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display an optional quantity field in the <em>Add to Cart</em> form.'),
    '#default_value' => variable_get('uc_product_add_to_cart_qty', FALSE),
    '#summary arguments' => array(
      t('The Quantity field in the <em>Add to Cart</em> form is enabled.'),
      t('The Quantity field in the <em>Add to Cart</em> form is disabled.'),
    ),
  );
  $form['uc_product_add_to_cart_teaser'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable <em>Add to cart</em> forms in product node teasers.'),
    '#default_value' => variable_get('uc_product_add_to_cart_teaser', TRUE),
    '#summary arguments' => array(
      t('The <em>Add to Cart</em> form is enabled in product teasers.'),
      t('The <em>Add to Cart</em> form is disabled in product teasers.'),
    ),
  );

  $form['uc_add_to_cart_text'] = array(
    '#type' => 'fieldset',
    '#title' => t('<em>Add to cart</em> button text'),
    '#description' => t('Use the textboxes to adjust the text of the submit button for <em>Add to Cart</em> forms in various places on the site.'),
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
  );
  $form['uc_add_to_cart_text']['uc_teaser_add_to_cart_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Teaser forms'),
    '#description' => t('For the form displayed on teasers and catalog pages.'),
    '#default_value' => variable_get('uc_teaser_add_to_cart_text', t('Add to cart')),
    '#summary' => t('Teaser and catalog pages: %text', array('%text' => variable_get('uc_teaser_add_to_cart_text', t('Add to cart')))),
    '#summary arguments' => array(FALSE),
  );
  $form['uc_add_to_cart_text']['uc_product_add_to_cart_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Product view'),
    '#description' => t('For the form displayed on the product view page.'),
    '#default_value' => variable_get('uc_product_add_to_cart_text', t('Add to cart')),
    '#summary' => t('Product view pages: %text', array('%text' => variable_get('uc_teaser_add_to_cart_text', t('Add to cart')))),
    '#summary arguments' => array(FALSE),
  );

  return system_settings_form($form);
}

/**
 * Allows store administrators to control what product information is relavent to their store.
 *
 * @see uc_product_settings_overview()
 * @see _uc_product_fields_summarize()
 * @see theme_uc_product_field_settings_form()
 * @see uc_product_field_settings_form_reset()
 * @see uc_product_field_settings_form_submit()
 * @ingroup forms
 */
function uc_product_field_settings_form($form, &$form_state) {
  $options = array(
    'model' => t('SKU'),
    'display_price' => t('Display price'),
    'list_price' => t('List price'),
    'cost' => t("Cost (seen only by 'administer products' permission)"),
    'sell_price' => t('Sell price'),
    'weight' => t('Weight'),
    'dimensions' => t('Dimensions'),
    'add_to_cart' => variable_get('uc_product_add_to_cart_text', t('Add to cart')),
  );

  $enabled = variable_get('uc_product_field_enabled', array(
    'model' => 'model',
    'display_price' => 'display_price',
    'sell_price' => 'sell_price',
    'add_to_cart' => 'add_to_cart',
  ));

  $weight = variable_get('uc_product_field_weight', array(
    'display_price' => -1,
    'model' => 0,
    'list_price' => 2,
    'cost' => 3,
    'sell_price' => 4,
    'weight' => 5,
    'dimensions' => 6,
    'add_to_cart' => 10,
  ));

  $fields = array();
  foreach ($options as $field => $value) {
    $fields[$field] = array(
      'title' => $value,
      'enabled' => isset($enabled[$field]) ? $enabled[$field] : FALSE,
      'weight' => $weight[$field],
    );
  }
  uasort($fields, 'uc_weight_sort');
  $form['fields'] = array('#tree' => TRUE);
  $form['fields']['#summary callback'] = '_uc_product_fields_summarize';
  $form['fields']['#summary arguments'] = array($options, $enabled, $weight);

  foreach ($fields as $label => $field) {
    $form['fields'][$label]['enabled'] = array(
      '#type' => 'checkbox',
      '#default_value' => $field['enabled'],
    );
    $form['fields'][$label]['title'] = array(
      '#markup' => $field['title'],
    );
    $form['fields'][$label]['weight'] = array(
      '#type' => 'weight',
      '#delta' => 10,
      '#default_value' => $field['weight'],
    );
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['actions']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to defaults'),
    '#submit' => array('uc_product_field_settings_form_reset'),
  );

  return $form;
}

/**
 * Summary callback for product fields settings.
 *
 * @see uc_product_field_settings_form()
 */
function _uc_product_fields_summarize($form, $options, $enabled, $weight) {

  $fields = array();
  foreach ($options as $field => $label) {
    if (isset($enabled[$field]) ? $enabled[$field] : FALSE) {
      $fields[$field] = array(
        'enabled' => $enabled[$field],
        'weight' => $weight[$field],
        'data' => $label,
      );
    }
  }
  uasort($fields, 'uc_weight_sort');

  foreach ($fields as $field) {
    $item[] = $field['data'];
  }

  $items[] = array(
    'data' => t('Displayed product fields:'),
    'children' => $item,
  );

  return $items;
}

/**
 * Displays the product field settings form.
 *
 * @see uc_product_field_settings_form()
 * @ingroup themeable
 */
function theme_uc_product_field_settings_form($variables) {
  $form = $variables['form'];

  $header = array(t('Enable'), t('Product field'), t('List position'));
  $rows = array();
  foreach (element_children($form['fields']) as $field) {
    $row = array();
    $row[] = drupal_render($form['fields'][$field]['enabled']);
    $row[] = drupal_render($form['fields'][$field]['title']);
    $row[] = drupal_render($form['fields'][$field]['weight']);
    $rows[] = $row;
  }
  $output = theme('table', array('header' => $header, 'rows' => $rows));

  $output .= drupal_render_children($form);
  return $output;
}

/**
 * Resets field settings to their default values.
 *
 * @see uc_product_field_settings_form()
 */
function uc_product_field_settings_form_reset($form, &$form_state) {
  variable_del('uc_product_field_enabled');
  variable_del('uc_product_field_weight');

  drupal_set_message(t('The configuration options have been reset to their default values.'));
}

/**
 * Saves the product fields settings.
 *
 * @see uc_product_field_settings_form()
 */
function uc_product_field_settings_form_submit($form, &$form_state) {
  $enabled = array();
  $weight = array();
  foreach ($form_state['values']['fields'] as $id => $field) {
    $enabled[$id] = $field['enabled'];
    $weight[$id] = $field['weight'];
  }
  variable_set('uc_product_field_enabled', $enabled);
  variable_set('uc_product_field_weight', $weight);

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Displays the settings form for all product features.
 *
 * @see _uc_product_features_summarize()
 * @ingroup forms
 */
function uc_product_feature_settings_form($form, &$form_state) {
  $titles = array();
  $features = module_invoke_all('uc_product_feature');
  foreach ($features as $feature) {
    $titles[] = $feature['title'];
  }
  if (empty($titles)) {
    $titles[] = '<em>' . t('No product features found.') . '</em>';
  }
  $form['features_list'] = array(
    '#markup' => '<div><b>' . t('The following product features are enabled')
               . ':</b><br />' . implode(', ', $titles) . '</div><br />',
    '#summary callback' => '_uc_product_features_summarize',
    '#summary arguments' => array($titles),
  );

  foreach ($features as $feature) {
    if (function_exists($feature['settings'])) {
      $form[$feature['id']] = array(
        '#type' => 'fieldset',
        '#title' => t('!feature settings', array('!feature' => $feature['title'])),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#summary callback' => 'uc_summarize_null',
      );
      $form[$feature['id']] = array_merge($form[$feature['id']], $feature['settings'](array(), $form_state));
    }
  }

  return system_settings_form($form);
}

/**
 * Summary callback for uc_product_feature_settings_form().
 */
function _uc_product_features_summarize($form, $titles) {

  foreach ($titles as $title) {
    $item[] = $title;
  }

  $items[] = array(
    'data' => t('The following features are enabled:'),
    'children' => $item,
  );

  return $items;

}

/**
 * Displays the product features tab on a product node edit form.
 */
function uc_product_features($node, $fid = NULL, $pfid = NULL) {
  drupal_set_title($node->title);

  $build = array();

  $header = array(t('Type'), t('Description'), t('Operations'));

  $features = uc_product_feature_load_multiple($node->nid);
  if (empty($features)) {
    $rows[] = array(
      array('data' => t('No features found for this product.'), 'colspan' => 3),
    );
  }
  else {
    foreach ($features as $feature) {
      $operations = array(
        l(t('edit'), 'node/' . $node->nid . '/edit/features/' . $feature->fid . '/' . $feature->pfid),
        l(t('delete'), 'node/' . $node->nid . '/edit/features/' . $feature->fid . '/' . $feature->pfid . '/delete'),
      );
      $rows[] = array(
        'data' => array(
          array('data' => uc_product_feature_data($feature->fid, 'title'), 'nowrap' => 'nowrap'),
          array('data' => $feature->description, 'width' => '100%'),
          array('data' => implode(' ', $operations), 'nowrap' => 'nowrap'),
        ),
        'valign' => 'top',
      );
    }
  }

  $build['features'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  $build['add_form'] = drupal_get_form('uc_product_feature_add_form', $node);

  return $build;
}

/**
 * Handles adding or editing product features.
 */
function uc_product_feature_edit($node, $fid, $pfid) {
  $func = uc_product_feature_data($fid, 'callback');

  if (function_exists($func)) {
    $form_state = array(
      'build_info' => array(
        'args' => array(
          $node,
        ),
      ),
      'wrapper_callback' => 'uc_product_feature_form',
    );

    if ($pfid == 'add') {
      $form_state['build_info']['args'][] = array();
      $build = drupal_build_form($func, $form_state);
    }
    elseif (intval($pfid) > 0) {
      $feature = uc_product_feature_load($pfid);

      if (isset($feature)) {
        $form_state['build_info']['args'][] = $feature;
        $build = drupal_build_form($func, $form_state);
      }
    }
    else {
      drupal_goto('node/' . $node->nid . '/edit/features');
    }
  }
  else {
    drupal_set_message(t('Error: Attempted to add a non-existent product feature type.'), 'error');
    drupal_goto('node/' . $node->nid . '/edit/features');
  }

  if (empty($build)) {
    drupal_set_message(t('Error: No form data was returned for that operation.'), 'error');
    drupal_goto('node/' . $node->nid . '/edit/features');
  }

  return $build;
}

/**
 * Adds the form for adding a product feature to the features tab.
 *
 * @see uc_product_feature_add_form_submit()
 * @see theme_uc_product_feature_add_form()
 * @ingroup forms
 */
function uc_product_feature_add_form($form, &$form_state, $node) {
  foreach (module_invoke_all('uc_product_feature') as $feature) {
    $options[$feature['id']] = $feature['title'];
  }
  ksort($options);

  $form['feature'] = array(
    '#type' => 'select',
    '#title' => t('Add a new feature'),
    '#options' => $options,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );

  return $form;
}

/**
 * Theme function for uc_product_feature_add_form().
 *
 * @see uc_product_feature_add_form()
 * @ingroup themeable
 */
function theme_uc_product_feature_add_form($variables) {
  return '<table class="add-feature"><tr><td>' . drupal_render_children($variables['form']) . '</td></tr></table>';
}

/**
 * Form submission handler for uc_product_feature_add_form().
 *
 * @see uc_product_feature_add_form()
 */
function uc_product_feature_add_form_submit($form, &$form_state) {
  $node = $form_state['build_info']['args'][0];

  $form_state['redirect'] = 'node/' . $node->nid . '/edit/features/' . $form_state['values']['feature'] . '/add';
}

/**
 * Confirmation form to delete a product feature.
 *
 * @see uc_product_feature_confirm_delete_submit()
 * @ingroup forms
 */
function uc_product_feature_confirm_delete($form, &$form_state, $node, $fid, $feature) {
  $question = $node->title;
  $description = t('Are you sure you wish to delete this %feature?', array('%feature' => uc_product_feature_data($fid, 'title')))
    . '<div><b>' . t('Description') . ':</b><br />' . $feature['description'] . '</div><br />';

  return confirm_form($form, $question, 'node/' . $node->nid . '/edit/features', $description, t('Delete'), t('Cancel'), 'pf_delete');
}

/**
 * Form submission handler for uc_product_feature_confirm_delete().
 *
 * @see uc_product_feature_confirm_delete()
 */
function uc_product_feature_confirm_delete_submit($form, &$form_state) {
  $node = $form_state['build_info']['args'][0];
  $feature = $form_state['build_info']['args'][2];

  if ($form_state['values']['pf_delete']) {
    uc_product_feature_delete($feature['pfid']);

    drupal_set_message(t('The product feature has been deleted.'));
  }

  $form_state['redirect'] = 'node/' . $node->nid . '/edit/features';
}

/**
 * Sets up imagefield and imagecache for products.
 *
 * @param $checks
 *   A bitmap describing the state of the image modules. Four checks are made
 *   to fill this bitmap: an imagefield exists, an imagefield is attached to
 *   product, the imagecache presets exist, and each preset has at least one
 *   action.
 *
 * @see uc_product_uc_store_status()
 */
function uc_product_image_defaults() {
  uc_product_add_default_image_field();

  drupal_set_message(t('Default image support configured for Ubercart products.'));

  drupal_goto('admin/store');
}

/**
 * Form builder for product classes.
 *
 * @see uc_product_class_form_validate()
 * @see uc_product_class_form_submit()
 * @ingroup forms
 */
function uc_product_class_form($form, &$form_state, $class = NULL) {
  if (!is_null($class)) {
    $classname = $class->name;
    $classdesc = $class->description;
    drupal_set_title($classname);
    $form['pcid'] = array(
      '#type' => 'hidden',
      '#value' => $class->pcid
    );
  }
  else {
    $classname = '';
    $classdesc = '';
    $form['pcid'] = array(
      '#type' => 'textfield',
      '#title' => t('Class ID'),
      '#required' => TRUE,
      '#max_length' => 32,
      '#description' => t('The machine-readable name of this content type. This text will be used for constructing the URL of the <em>create content</em> page for this content type. This name may consist only of lowercase letters, numbers, and underscores. Dashes are not allowed. Underscores will be converted into dashes when constructing the URL of the <em>create content</em> page. This name must be unique to this content type.'),
    );
  }

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Class name'),
    '#default_value' => $classname,
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('This text describes the content type created for this product class to administrators.'),
    '#default_value' => $classdesc,
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Ensures the new product class is unique.
 *
 * @see uc_product_class_form()
 * @see uc_product_class_form_submit()
 */
function uc_product_class_form_validate($form, &$form_state) {
  if ($form['pcid']['#type'] == 'textfield') {
    $type = node_type_get_type($form_state['values']['pcid']);
    if ($type) {
      if ($type->base == 'uc_product') {
        form_set_error('pcid', t('This product class already exists.'));
      }
      elseif ($type->custom == 0) {
        form_set_error('pcid', t('This is a node type provided by another module. Only custom node types may become product classes.'));
      }
    }
  }
}

/**
 * Form submission handler for uc_product_class_form().
 *
 * @see uc_product_class_form()
 * @see uc_product_class_form_validate()
 */
function uc_product_class_form_submit($form, &$form_state) {
  $is_new = $form['pcid']['#type'] == 'textfield';

  $pcid = $form_state['values']['pcid'];

  // Convert whitespace to underscores, and remove other non-alphanumeric characters.
  $pcid = preg_replace(array('/\s+/', '/\W/'), array('_', ''), strtolower($pcid));

  $result = db_merge('uc_product_classes')
    ->key(array('pcid' => $pcid))
    ->fields(array(
      'name' => $form_state['values']['name'],
      'description' => $form_state['values']['description'],
    ))
    ->execute();

  uc_product_node_info(TRUE);

  if ($result == MergeQuery::STATUS_INSERT) {
    variable_set('node_options_' . $pcid, variable_get('node_options_product', array('status', 'promote')));

    if (module_exists('comment')) {
      variable_set('comment_' . $pcid, variable_get('comment_product', COMMENT_NODE_OPEN));
    }

    module_invoke_all('uc_product_class', $pcid, 'insert');
  }
  else {
    module_invoke_all('uc_product_class', $pcid, 'update');
  }

  node_types_rebuild();
  if ($is_new) {
    $type = node_type_get_type($pcid);
    node_add_body_field($type, t('Description'));
    uc_product_add_default_image_field($pcid);
  }
  menu_rebuild();

  drupal_set_message(t('Product class saved.'));
}

/**
 * Confirms the deletion of a product class.
 *
 * @see uc_product_class_delete_confirm_submit()
 */
function uc_product_class_delete_confirm($form, &$form_state, $class) {
  $form['pcid'] = array(
    '#type' => 'value',
    '#value' => $class->pcid,
  );

  $question = t('Are you sure you want to delete the %type product class?', array('%type' => $class->pcid));
  $description = t('The node type associated with this product class will revert to a standard node type.');

  // Find out how many nodes of this class exist and add to the description.
  $count = db_query("SELECT COUNT(*) FROM {node} WHERE type = :pcid", array(':pcid' => $class->pcid))->fetchField();

  if ($count > 0) {
    $description .= '<br />' . format_plural($count, 'There is 1 node of this type.', 'There are @count nodes of this type.');
  }

  return confirm_form($form, $question, 'admin/store/products/classes', $description, t('Delete'), t('Cancel'));
}

/**
 * Form submission handler for uc_product_class_delete_confirm().
 *
 * @see uc_product_class_delete_confirm()
 */
function uc_product_class_delete_confirm_submit($form, &$form_state) {
  $type = node_type_get_type($form_state['values']['pcid']);
  $type->base = 'node_content';
  $type->custom = 1;
  node_type_save($type);

  db_delete('uc_product_classes')
    ->condition('pcid', $form_state['values']['pcid'])
    ->execute();
  module_invoke_all('uc_product_class', $form_state['values']['pcid'], 'delete');
  uc_product_node_info(TRUE);
  node_types_rebuild();
  menu_rebuild();

  drupal_set_message(t('Product class %type deleted.', array('%type' => $form_state['values']['pcid'])));

  $form_state['redirect'] = 'admin/store/products/classes';
}
