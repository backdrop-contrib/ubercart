<?php
/**
 * @file
 * Product administration menu items.
 */

/**
 * Displays a list of product classes.
 */
function uc_product_class_default() {
  $classes = uc_product_class_load();
  $header = array(t('Class ID'), t('Name'), t('Description'), t('Operations'));
  $rows = array();
  foreach ($classes as $class) {
    $links[] = array(
      'title' => t('edit'),
      'href' => 'admin/store/products/classes/' . $class->pcid . '/edit',
    );
    if (empty($class->locked)) {
      $links[] = array(
        'title' => t('delete'),
        'href' => 'admin/store/products/classes/' . $class->pcid . '/delete',
      );
    }
    $operations = array(
      '#type' => 'operations',
      '#links' => $links,
    );
    $rows[] = array(
      check_plain($class->pcid),
      check_plain($class->name),
      filter_xss_admin($class->description),
      backdrop_render($operations),
    );
  }

  $build = array();
  $build['product_classes'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No product classes have been defined yet.'),
  );
  $build['header'] = array(
    '#markup' => '<h2>' . t('Add a class') . '</h2>',
  );
  $build['form'] = backdrop_get_form('uc_product_class_form');

  return $build;
}

/**
 * Form to change product settings.
 *
 * Note that modules that alter this form to store their own settings on this
 * page (like uc_attribute and uc_product_kit) should store their settings in
 * their own config file.
 *
 * @ingroup forms
 */
function uc_product_settings_form($form, &$form_state) {
  $config = config('uc_product.settings');

  // Other modules, including uc_attribute and uc_product_kit, add their own
  // settings to this form via hook_form_FORM_ID_alter(). For this reason, we
  // don't set $form['#config'] and use our own submission function rather than
  // system_settings_form_submit() to avoid stuffing everything into
  // uc_product.settings config.

  // Put fieldsets into vertical tabs
  $form['product-settings'] = array('#type' => 'vertical_tabs');

  $form['product'] = array(
    '#type' => 'fieldset',
    '#title' => t('Product settings'),
    '#group' => 'product-settings',
    '#weight' => -10,
  );

  // Setting for the product image field: default or custom (or none).
  if (isset($form_state['values']['uc_image_product'])) {
    $default_product_image_field = $form_state['values']['uc_image_product'];
  }
  else {
    $default_product_image_field = $config->get('uc_image_product');
  }
  if (is_null($default_product_image_field)) {
    $default_product_image_field = '';
  }
  if ($default_product_image_field == 'uc_product_image') {
    $instances = field_info_instances('node', 'product');
    $mismatch = !isset($instances[$default_product_image_field]);
  }
  else {
    $mismatch = FALSE;
  }
  $uc_product_image_description = t('Product image support provided by Ubercart.');
  if ($mismatch) {
    $uc_product_image_description .= ' ' . t('Not configured yet. <a href="!url">Automatically configure Ubercart products for core image support</a>.', array('!url' => url('admin/store/settings/products/defaults')));
  }
  $form['product']['uc_image_product_wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array('id' => 'uc-image-product-wrapper'),
  );
  $form['product']['uc_image_product_wrapper']['uc_image_product'] = array(
    '#type' => 'radios',
    '#title' => t('Product image field'),
    '#options' => array(
      'uc_product_image' => t('Default Ubercart image field'),
      '' => t('No images needed or other image support'),
    ),
    'uc_product_image' => array(
      '#description' => $uc_product_image_description,
    ),
    '' => array(
      '#description' => t('Select this option if your products do not need images or you have implemented your own image support.'),
    ),
    '#default_value' => $default_product_image_field,
    '#ajax' => array(
      'callback' => 'uc_product_settings_form_image_product_callback',
      'wrapper' => 'uc-image-product-wrapper',
    ),
  );

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
    $options['none'] = t("Don't use any image widgets.");
  }
  $form['product']['uc_product_image_widget'] = array(
    '#type' => 'radios',
    '#title' => t('Product image widget'),
    '#description' => t('For image fields that use the "Ubercart product" field formatter, the selected widget will be used to display a zoomed version of product images when they are clicked.'),
    '#options' => $options,
    '#default_value' => $config->get('uc_product_image_widget'),
  );

  if (module_exists('uc_cart')) {
    $form['product']['uc_product_add_to_cart_qty'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display an optional quantity field in the <em>Add to Cart</em> form.'),
      '#default_value' => $config->get('uc_product_add_to_cart_qty'),
    );
    $form['product']['uc_product_update_node_view'] = array(
      '#type' => 'checkbox',
      '#title' => t('Update product display based on customer selections'),
      '#default_value' => $config->get('uc_product_update_node_view'),
      '#description' => t('Check this box to dynamically update the display of product information such as display-price or weight based on customer input on the add-to-cart form (e.g. selecting a particular attribute option).'),
    );
  }

  foreach (module_invoke_all('uc_product_feature') as $feature) {
    if (isset($feature['settings']) && function_exists($feature['settings'])) {
      $form[$feature['id']] = array(
        '#type' => 'fieldset',
        '#title' => t('@feature settings', array('@feature' => $feature['title'])),
        '#group' => 'product-settings',
      );
      $form[$feature['id']] += $feature['settings'](array(), $form_state);

      if (function_exists($feature['settings'] . '_validate')) {
        $form['#validate'][] = $feature['settings'] . '_validate';
      }
      if (function_exists($feature['settings'] . '_submit')) {
        $form['#submit'][] = $feature['settings'] . '_submit';
      }
    }
  }

  $form = system_settings_form($form);

  // Swap in our own form submission function for the default system settings
  // form submisison handler. Ours should run before any other submission
  // functions because it cleans the form values for client modules to extract
  // and store their own settings.

  unset($form['#submit'][array_search('system_settings_form_submit', $form['#submit'])]);
  array_unshift($form['#submit'], 'uc_product_settings_form_submit');
  return $form;
}

/**
 * Callback for uc_product_image controls.
 */
function uc_product_settings_form_image_product_callback($form, &$form_state) {
  return $form['product']['uc_image_product_wrapper'];
}

/**
 * Submission function for product settings.
 *
 * @see system_settings_form_submit()
 */
function uc_product_settings_form_submit($form, &$form_state) {
  form_state_values_clean($form_state);

  $config = config('uc_product.settings');
  $config->set('uc_image_product', $form_state['values']['uc_image_product']);
  $config->set('uc_product_image_widget', $form_state['values']['uc_product_image_widget']);
  if (module_exists('uc_cart')) {
    $config->set('uc_product_add_to_cart_qty', $form_state['values']['uc_product_add_to_cart_qty']);
    $config->set('uc_product_update_node_view', $form_state['values']['uc_product_update_node_view']);
  }
  $config->save();

  backdrop_set_message(t('The configuration options have been saved.'));
}

/**
 * Displays the product features tab on a product node edit form.
 */
function uc_product_features($node, $fid = NULL, $pfid = NULL) {
  backdrop_set_title($node->title);

  $header = array(t('Type'), t('Description'), t('Operations'));
  $rows = array();

  $features = uc_product_feature_load_multiple($node->nid);
  foreach ($features as $feature) {
    $links = array(
      'edit' => array(
        'title' => t('edit'),
        'href' => 'node/' . $node->nid . '/edit/features/' . $feature->fid . '/' . $feature->pfid,
      ),
      'delete' => array(
        'title' => t('delete'),
        'href' => 'node/' . $node->nid . '/edit/features/' . $feature->fid . '/' . $feature->pfid . '/delete',
      ),
    );
    $operations = array(
      '#type' => 'operations',
      '#links' => $links,
    );
    $rows[] = array(
      array('data' => uc_product_feature_data($feature->fid, 'title')),
      array('data' => $feature->description),
      backdrop_render($operations),
    );
  }

  $build['features'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('class' => array('uc-product-features')),
    '#empty' => t('No features found for this product.'),
  );
  $build['add_form'] = backdrop_get_form('uc_product_feature_add_form', $node);

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
      $build = backdrop_build_form($func, $form_state);
    }
    elseif (intval($pfid) > 0) {
      $feature = uc_product_feature_load($pfid);

      if (isset($feature)) {
        $form_state['build_info']['args'][] = $feature;
        $build = backdrop_build_form($func, $form_state);
      }
    }
    else {
      backdrop_goto('node/' . $node->nid . '/edit/features');
    }
  }
  else {
    backdrop_set_message(t('Error: Attempted to add a non-existent product feature type.'), 'error');
    backdrop_goto('node/' . $node->nid . '/edit/features');
  }

  if (empty($build)) {
    backdrop_set_message(t('Error: No form data was returned for that operation.'), 'error');
    backdrop_goto('node/' . $node->nid . '/edit/features');
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
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @see uc_product_feature_add_form()
 * @ingroup themeable
 */
function theme_uc_product_feature_add_form($variables) {
  return '<table class="add-feature"><tr><td>' . backdrop_render_children($variables['form']) . '</td></tr></table>';
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
  $description = t('Are you sure you wish to delete this %feature?', array('%feature' => uc_product_feature_data($fid, 'title')))
    . '<div><b>' . t('Description') . ':</b><br />' . $feature['description'] . '</div><br />';

  return confirm_form($form, check_plain($node->title), 'node/' . $node->nid . '/edit/features', $description, t('Delete'), t('Cancel'), 'pf_delete');
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

    backdrop_set_message(t('The product feature has been deleted.'));
  }

  $form_state['redirect'] = 'node/' . $node->nid . '/edit/features';
}

/**
 * Sets up image field for products.
 *
 * @see uc_product_settings_form()
 */
function uc_product_image_defaults() {
  uc_product_add_default_image_field();

  backdrop_set_message(t('Default image support configured for Ubercart products.'));

  backdrop_goto('admin/store/settings/products');
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
    backdrop_set_title($classname);
    $form['pcid'] = array(
      '#type' => 'hidden',
      '#value' => $class->pcid,
    );
  }
  else {
    $classname = '';
    $classdesc = '';
    $form['pcid'] = array(
      '#type' => 'textfield',
      '#title' => t('Class ID'),
      '#required' => TRUE,
      '#maxlength' => 32,
      '#description' => t('The machine-readable name of this content type. This text will be used for constructing the URL of the <em>create content</em> page for this content type. This name may consist only of lowercase letters, numbers, and underscores. Dashes are not allowed. Underscores will be converted into dashes when constructing the URL of the <em>create content</em> page. This name must be unique to this content type.'),
    );
  }

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Class name'),
    '#description' => t('The human-readable name of this content type as it should appear on the !create_content page.  There are no character restrictions on this name.', array('!create_content' => l(t('Create content'), 'node/add'))),
    '#default_value' => $classname,
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('This text describes the content type to administrators.'),
    '#default_value' => $classdesc,
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
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

  $type = new \stdClass();
  $type->type = $pcid;
  $type->name = $form_state['values']['name'];
  $type->base = 'uc_product';
  $type->module = 'uc_product';
  $type->description = $form_state['values']['description'];
  $type = node_type_set_defaults($type);
  node_type_save($type);

  uc_product_node_info(TRUE);

  if ($result == MergeQuery::STATUS_INSERT) {
    module_invoke_all('uc_product_class', $pcid, 'insert');
  }
  else {
    module_invoke_all('uc_product_class', $pcid, 'update');
  }

  node_type_cache_reset();
  if ($is_new) {
    $type = node_type_get_type($pcid);
    node_add_body_field($type, t('Description'));
    uc_product_add_default_image_field($pcid);
  }
  menu_rebuild();

  backdrop_set_message(t('Product class saved.'));
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
  $type->module = 'node';
  $type->custom = 1;
  node_type_save($type);

  db_delete('uc_product_classes')
    ->condition('pcid', $form_state['values']['pcid'])
    ->execute();
  module_invoke_all('uc_product_class', $form_state['values']['pcid'], 'delete');
  // TODO migrate This needed?
  uc_product_node_info(TRUE);
  node_type_cache_reset();
  menu_rebuild();

  backdrop_set_message(t('Product class %type deleted.', array('%type' => $form_state['values']['pcid'])));

  $form_state['redirect'] = 'admin/store/products/classes';
}
