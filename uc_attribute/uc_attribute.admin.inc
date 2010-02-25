<?php
// $Id$

/**
 * @file
 * Attribute administration menu items.
 *
 */

/**
 * Display a paged list and overview of existing product attributes.
 */
function uc_attribute_admin() {
  $header = array(
    array('data' => t('Name'), 'field' => 'a.name', 'sort' => 'asc'),
    array('data' => t('Label'), 'field' => 'a.label'),
    t('Required'),
    array('data' => t('List position'), 'field' => 'a.ordering'),
    t('Number of options'),
    t('Display type'),
    array('data' => t('Operations'), 'colspan' => 3),
  );

  $display_types = _uc_attribute_display_types();

  $query = db_select('uc_attributes', 'a')->extend('PagerDefault')->extend('TableSort')
    ->orderByHeader($header)
    ->limit(30);
  $table_ao = $query->leftJoin('uc_attribute_options', 'ao', 'a.aid = ao.aid');

  $a_aid = $query->addField('a', 'aid');
  $a_name = $query->addField('a', 'name');
  $a_label = $query->addField('a', 'label');
  $a_required = $query->addField('a', 'required');
  $a_ordering = $query->addField('a', 'ordering');
  $a_display = $query->addField('a', 'display');
  $ao_options = $query->addExpression('COUNT(ao.oid)', 'options');

  $query->groupBy($a_aid)
    ->groupBy($a_name)
    ->groupBy($a_label)
    ->groupBy($a_required)
    ->groupBy($a_ordering)
    ->groupBy($a_display);

  $result = $query->execute();
  foreach ($result as $attr) {
    if (empty($attr->label)) {
      $attr->label = $attr->name;
    }
    $rows[] = array(
      check_plain($attr->name),
      check_plain($attr->label),
      $attr->required == 1 ? t('Yes') : t('No'),
      array('data' => $attr->ordering, 'align' => 'center'),
      array('data' => $attr->options, 'align' => 'center'),
      $display_types[$attr->display],
      l(t('edit'), 'admin/store/attributes/'. $attr->aid .'/edit'),
      l(t('options'), 'admin/store/attributes/'. $attr->aid .'/options'),
      l(t('delete'), 'admin/store/attributes/'. $attr->aid .'/delete'),
    );
  }

  if (count($rows) == 0) {
    $rows[] = array(
      array('data' => t('No product attributes have been added yet.'), 'colspan' => '9')
    );
  }

  $build['attributes'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  $build['pager'] = array(
    '#theme' => 'pager',
  );
  $build['add_more'] = array(
    '#theme' => 'link',
    '#text' => t('Add an attribute'),
    '#href' => 'admin/store/attributes/add',
  );

  return $build;
}

/**
 * Form builder for product attributes.
 *
 * @ingroup forms
 * @see uc_attribute_form_submit()
 */
function uc_attribute_form($form, &$form_state, $attribute = NULL) {
  // If an attribute specified, add its ID as a hidden value.
  if (!empty($attribute)) {
    $form['aid'] = array('#type' => 'hidden', '#value' => $attribute->aid);
    drupal_set_title(t('Edit attribute: %name', array('%name' => $attribute->name)));
  }

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('The name of the attribute used in administrative forms'),
    '#default_value' => $attribute->name,
    '#required' => TRUE,
  );
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#description' => t("Enter a label that customers will see instead of the attribute name. Use &lt;none&gt; if you don't want a title to appear at all."),
    '#default_value' => empty($attribute->label) ? $attribute->name : $attribute->label,
  );
  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Help text'),
    '#description' => t('<b>Optional.</b> Enter the help text that will display beneath the attribute on product add to cart forms.'),
    '#default_value' => $attribute->description,
    '#maxlength' => 255,
  );
  $form['required'] = array(
    '#type' => 'checkbox',
    '#title' => t('Make this attribute required, forcing the customer to choose an option.'),
    '#description' => t('Selecting this for an attribute will disregard any default option you specify.<br />May be overridden at the product level.'),
    '#default_value' => $attribute->required,
  );
  $form['display'] = array(
    '#type' => 'select',
    '#title' => t('Display type'),
    '#description' => t('This specifies how the options for this attribute will be presented.<br />May be overridden at the product level.'),
    '#options' => _uc_attribute_display_types(),
    '#default_value' => isset($attribute->display) ? $attribute->display : 1,
  );
  $form['ordering'] = array(
    '#type' => 'weight',
    '#title' => t('List position'),
    '#description' => t('Multiple attributes on an add to cart form are sorted by this value and then by their name.<br />May be overridden at the product level.'),
    '#default_value' => isset($attribute->ordering) ? $attribute->ordering : 0,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#suffix' => l(t('Cancel'), 'admin/store/attributes'),
  );

  return $form;
}

/**
 * @see uc_attribute_add_form()
 */
function uc_attribute_form_submit($form, &$form_state) {
  if (!empty($form_state['values']['aid'])) {
    drupal_write_record('uc_attributes', $form_state['values'], 'aid');
  }
  else {
    drupal_write_record('uc_attributes', $form_state['values']);
  }

  $form_state['redirect'] = 'admin/store/attributes';
}

/**
 * Confirm the deletion of the given attribute.
 *
 * @see uc_attribute_delete_confirm_submit()
 */
function uc_attribute_delete_confirm($form_state, $attribute) {
  // If we got a bunk attribute, kick out an error message.
  if (empty($attribute)) {
    drupal_set_message(t('There is no attribute with that ID.'), 'error');
    drupal_goto('admin/store/attributes');
  }

  $form['aid'] = array('#type' => 'value', '#value' => $attribute->aid);
  $form['#redirect'] = 'admin/store/attributes';

  $count = db_query("SELECT COUNT(*) FROM {uc_product_attributes} WHERE aid = :aid", array(':aid' => $attribute->aid))->fetchField();

  $output = confirm_form($form, t('Are you sure you want to delete the attribute %name?', array('%name' => $attribute->name)),
              'admin/store/attributes', format_plural($count, 'There is @count product with this attribute.', 'There are @count products with this attribute.'),
              t('Delete'), t('Cancel'));

  return $output;
}

/**
 * @see uc_attribute_delete_confirm()
 */
function uc_attribute_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $attribute = uc_attribute_load($form_state['values']['aid']);
    $options = array_keys($attribute->options);

    db_delete('uc_class_attribute_options')
      ->condition('oid', $options, 'IN')
      ->execute();

    db_delete('uc_class_attributes')
      ->condition('aid', $form_state['values']['aid'])
      ->execute();

    db_delete('uc_product_options')
      ->condition('oid', $options, 'IN')
      ->execute();

    $nodes = db_query("SELECT nid FROM {uc_product_attributes} WHERE aid = :aid", array(':aid' => $attribute->aid))->fetchCol();
    db_delete('uc_product_adjustments')
      ->condition('nid', $nodes, 'IN')
      ->execute();

    db_delete('uc_product_attributes')
      ->condition('aid', $form_state['values']['aid'])
      ->execute();

    db_delete('uc_attribute_options')
      ->condition('aid', $form_state['values']['aid'])
      ->execute();

    db_delete('uc_attributes')
      ->condition('aid', $form_state['values']['aid'])
      ->execute();

    drupal_set_message(t('Product attribute deleted.'));
  }
}

/**
 * Change the display of attribute option prices.
 *
 * @ingroup forms
 */
function uc_attribute_admin_settings() {
  $form = array();

  $form['uc_attribute_option_price_format'] = array('#type' => 'radios',
    '#title' => t('Option price format'),
    '#default_value' => variable_get('uc_attribute_option_price_format', 'adjustment'),
    '#options' => array('none' => t('Do not display'),
      'adjustment' => t('Display price adjustment'),
      'total' => t('Display total price'),
    ),
    '#description' => t('Formats the price in the attribute selection form when the customer adds a product to their cart. The total price will only be displayed on products with only one price affecting attribute.'),
  );

  return system_settings_form($form);
}

/**
 * Display options and the modifications to products they represent.
 *
 * @ingroup forms
 * @see
 *   uc_attribute_options_form_validate()
 *   uc_attribute_options_form_submit()
 */
function uc_attribute_options_form($form, &$form_state, $attribute) {
  // Set an appropriate title.
  drupal_set_title(t('Options for %name', array('%name' => $attribute->name)));

  // Store the attribute ID in the form array.
  $form['aid'] = array(
    '#type' => 'value',
    '#value' => $attribute->aid,
  );

  $context = array(
    'revision' => 'themed',
    'type' => 'attribute_option',
    'subject' => array(
      'attribute' => $attribute,
    ),
  );

  // Loop through all the options on an attribute.
  foreach ($attribute->options as $key => $data) {
    $form['options'][$key] = array(
      'name' => array(
        '#markup' => check_plain($data->name),
      ),
      'cost' => array(
        '#markup' => $data->cost,
      ),
      'price' => array(
        '#markup' => $data->price,
      ),
      'weight' => array(
        '#markup' => $data->weight,
      ),
      'ordering' => array(
        '#type' => 'weight',
        '#delta' => 50,
        '#default_value' => $data->ordering,
        '#attributes' => array('class' => array('uc-attribute-option-table-ordering')),
      ),
      'edit' => array(
        '#theme' => 'link',
        '#text' => t('edit'),
        '#href' => 'admin/store/attributes/'. $attribute->aid .'/options/'. $key .'/edit',
      ),
      'delete' => array(
        '#theme' => 'link',
        '#text' => t('delete'),
        '#href' => 'admin/store/attributes/'. $attribute->aid .'/options/'. $key .'/delete',
      ),
    );

    $context['subject']['option'] = $data;
    $context['field'] = 'cost';
    $form['options'][$key]['cost']['#markup'] = uc_price($data->cost, $context);

    $context['field'] = 'price';
    $form['options'][$key]['price']['#markup'] = uc_price($data->price, $context);
  }

  if (count($form['options'])) {
    $form['options']['#tree'] = TRUE;

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save changes'),
      '#weight' => 10,
    );
  }

  return $form;
}

function uc_attribute_options_form_submit($form, &$form_state) {
  foreach ($form_state['values']['options'] as $oid => $option) {
    db_update('uc_attribute_options')
      ->fields(array(
        'ordering' => $option['ordering'],
      ))
      ->condition('oid', $oid)
      ->execute();
  }

  drupal_set_message(t('The changes have been saved.'));
}

/**
 * Format an attribute and its options.
 *
 * @ingroup themeable
 */
function theme_uc_attribute_options_form($variables) {
  $form = $variables['form'];

  $header = array(t('Name'), t('Default cost'), t('Default price'), t('Default weight'), array('data' => t('List position'), 'sort' => 'asc'), array('data' => t('Operations'), 'colspan' => 2));

  if (count(element_children($form['options'])) > 0) {
    foreach (element_children($form['options']) as $oid) {
      $row = array(
        drupal_render($form['options'][$oid]['name']),
        drupal_render($form['options'][$oid]['cost']),
        drupal_render($form['options'][$oid]['price']),
        drupal_render($form['options'][$oid]['weight']),
        drupal_render($form['options'][$oid]['ordering']),
        drupal_render($form['options'][$oid]['edit']),
        drupal_render($form['options'][$oid]['delete']),
      );

      $rows[] = array(
        'data' => $row,
        'class' => array('draggable'),
      );
    }
  }
  else {
    $rows[] = array(
      array('data' => t('No options for this attribute have been added yet.'), 'colspan' => 7),
    );
  }

  drupal_add_tabledrag('uc-attribute-option-table', 'order', 'sibling', 'uc-attribute-option-table-ordering');

  $output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'uc-attribute-option-table')));

  $output .= drupal_render_children($form);

  $output .= l(t('Add an option'), 'admin/store/attributes/'. $form['aid']['#value'] .'/options/add');

  return $output;
}

/**
 * Form builder for attribute options.
 *
 * @ingroup forms
 * @see uc_attribute_option_form_validate()
 * @see uc_attribute_option_form_submit()
 */
function uc_attribute_option_form($form, &$form_state, $attribute, $option = NULL) {
  // If we got a bunk attribute, kick out an error message.
  if (empty($attribute)) {
    drupal_set_message(t('There is no attribute with that ID.'), 'error');
    drupal_goto('admin/store/attributes');
  }
  $aid = $attribute->aid;

  $form['aid'] = array('#type' => 'hidden', '#value' => $aid);

  if (!empty($option)) {
    $form['oid'] = array('#type' => 'hidden', '#value' => $option->oid);
    drupal_set_title(t('Edit option: %name', array('%name' => $option->name)));
  }
  else {
    drupal_set_title(t('Options for %name', array('%name' => $attribute->name)));
  }

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('This name will appear to customers on product add to cart forms.'),
    '#default_value' => $option->name,
    '#required' => TRUE,
    '#weight' => 0,
  );
  $form['ordering'] = array(
    '#type' => 'weight',
    '#title' => t('List position'),
    '#description' => t('Options will be listed sorted by this value and then by their name.<br />May be overridden at the product level.'),
    '#default_value' => isset($option->ordering) ? $option->ordering : 0,
    '#weight' => 4,
  );
  $form['adjustments'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default adjustments'),
    '#description' => t('Enter a positive or negative value for each adjustment applied when this option is selected.<br />Any of these may be overriden at the product level.'),
    '#collapsible' => FALSE,
    '#weight' => 8,
  );
  $form['adjustments']['cost'] = array(
    '#type' => 'textfield',
    '#title' => t('Cost'),
    '#default_value' => uc_store_format_price_field_value($option->cost),
    '#weight' => 1,
  );
  $form['adjustments']['price'] = array(
    '#type' => 'textfield',
    '#title' => t('Price'),
    '#default_value' => uc_store_format_price_field_value($option->price),
    '#weight' => 2,
  );
  $form['adjustments']['weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Weight'),
    '#default_value' => $option->weight,
    '#weight' => 3,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#suffix' => l(t('Cancel'), 'admin/store/attributes/'. $aid .'/options'),
    '#weight' => 10,
  );

  return $form;
}

/**
 * Validate number formats.
 *
 * @see uc_attribute_option_form()
 */
function uc_attribute_option_form_validate($form, &$form_state) {
  $pattern = '/^-?\d*(\.\d*)?$/';
  $price_error = t('This must be in a valid number format. No commas and only one decimal point.');
  if (!is_numeric($form_state['values']['cost']['#value']) && !preg_match($pattern, $form_state['values']['cost']['#value'])) {
    form_set_error('cost', $price_error);
  }
  if (!is_numeric($form_state['values']['price']['#value']) && !preg_match($pattern, $form_state['values']['price']['#value'])) {
    form_set_error('price', $price_error);
  }
  if (!is_numeric($form_state['values']['weight']['#value']) && !preg_match($pattern, $form_state['values']['weight']['#value'])) {
    form_set_error('weight', $price_error);
  }
}

/**
 * @see uc_attribute_option_form().
 */
function uc_attribute_option_form_submit($form, &$form_state) {
  if (!isset($form_state['values']['oid'])) {
    drupal_write_record('uc_attribute_options', $form_state['values']);
    drupal_set_message(t('Created new option %option.', array('%option' => $form_state['values']['name'])));
    watchdog('uc_attribute', 'Created new option %option.', array('%option' => $form_state['values']['name']), WATCHDOG_NOTICE, 'admin/store/attributes/'. $form_state['values']['aid'] .'/options/add');
    $form_state['redirect'] = 'admin/store/attributes/'. $form_state['values']['aid'] .'/options/add';
  }
  else {
    drupal_write_record('uc_attribute_options', $form_state['values'], array('aid', 'oid'));
    drupal_set_message(t('Updated option %option.', array('%option' => $form_state['values']['name'])));
    watchdog('uc_attribute', 'Updated option %option.', array('%option' => $form_state['values']['name']), WATCHDOG_NOTICE, 'admin/store/attributes/'. $form_state['values']['aid'] .'/options/'. $form_state['values']['oid']);
    $form_state['redirect'] = 'admin/store/attributes/'. $form_state['values']['aid'] .'/options';
  }
}

/**
 * Confirm deletion of the given attribute option.
 *
 * @see uc_attribute_option_delete_confirm_submit()
 */
function uc_attribute_option_delete_confirm($form_state, $attribute, $option) {
  if (empty($option)) {
    drupal_set_message(t('There is no option with that ID.'), 'error');
    drupal_goto('admin/store/attributes/'. $attribute->aid .'/options');
  }
  $aid = $attribute->aid;
  $oid = $option->oid;

  $form['aid'] = array('#type' => 'value', '#value' => $aid);
  $form['oid'] = array('#type' => 'value', '#value' => $oid);

  $output = confirm_form($form, t('Are you sure you want to delete the option %name?', array('%name' => $option->name)),
             'admin/store/attributes/'. $aid .'/options', '',
             t('Delete'), t('Cancel'));

  return $output;
}

/**
 * @see uc_attribute_option_delete_confirm().
 */
function uc_attribute_option_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $match = 'i:'. $form_state['values']['aid'] .';s:'. strlen($form_state['values']['oid']) .':"'. $form_state['values']['oid'] .'";';

    db_delete('uc_product_adjustments')
      ->condition('combination', '%' . db_like($match) . '%', 'LIKE')
      ->execute();

    $select = db_select('uc_attribute_options', 'ao')
      ->where('{uc_class_attribute_options}.oid = ao.oid')
      ->condition('ao.oid', $form_state['values']['oid']);
    $select->addExpression('1');
    db_delete('uc_class_attribute_options')
      ->condition('', $select, 'EXISTS')
      ->execute();

    $select = db_select('uc_attribute_options', 'ao')
      ->where('{uc_product_options}.oid = ao.oid')
      ->condition('ao.oid', $form_state['values']['oid']);
    $select->addExpression('1');
    db_delete('uc_product_options')
      ->condition('', $select, 'EXISTS')
      ->execute();

    db_delete('uc_attribute_options')
      ->condition('oid', $form_state['values']['oid'])
      ->execute();
  }

  $form_state['redirect'] = 'admin/store/attributes/'. $form_state['values']['aid'] .'/options';
}

/**
 * Form to associate attributes with products or classes.
 *
 * @ingroup forms
 * @see
 *   theme_uc_object_attributes_form()
 *   uc_object_attributes_form_submit()
 */
function uc_object_attributes_form($form, &$form_state, $object, $type, $view = 'overview') {
  switch ($type) {
    case 'class':
      $class = $object;
      $id = $class->pcid;
      if (empty($class->name)) {
        drupal_goto('admin/store/products/classes/'. $id);
      }
      drupal_set_title(check_plain($class->name));
      $attributes = uc_class_get_attributes($id);
      break;
    case 'product':
    default:
      $product = $object;
      $id = $product->nid;
      if (empty($product->title)) {
        drupal_goto('node/'. $id);
      }
      drupal_set_title(check_plain($product->title));
      $attributes = uc_product_get_attributes($id);
  }

  $used_aids = array();
  $used_labels = array();
  foreach ($attributes as $attribute) {
    $used_aids[] = $attribute->aid;
    if (!empty($attribute->label)) {
      $used_labels[] = $attribute->label;
    }
  }

  if ($view == 'overview') {
    $form['#tree'] = TRUE;

    $context = array(
      'revision' => 'themed',
      'type' => 'attribute_option',
    );
    if (count($attributes) > 0) {
      foreach ($attributes as $attribute) {
        $option = $attribute->options[$attribute->default_option];
        $context['subject'] = array(
          'attribute' => $attribute,
          'option' => $option,
        );

        $form['attributes'][$attribute->aid] = array(
          'remove' => array(
            '#type' => 'checkbox',
            '#default_value' => 0,
          ),
          'name' => array(
            '#markup' => check_plain($attribute->name),
          ),
          'label' => array(
            '#type' => 'textfield',
            '#default_value' => empty($attribute->label) ? $attribute->name : $attribute->label,
            '#size' => 6,
          ),
          'option' => array(
            '#markup' => $option ? (check_plain($option->name) .' ('. uc_price($option->price, $context) .')' ) : t('n/a'),
          ),
          'required' => array(
            '#type' => 'checkbox',
            '#default_value' => $attribute->required,
          ),
          'ordering' => array(
            '#type' => 'weight',
            '#default_value' => $attribute->ordering,
            '#attributes' => array('class' => array('uc-attribute-table-weight')),
          ),
          'display' => array(
            '#type' => 'select',
            '#default_value' => $attribute->display,
            '#options' => _uc_attribute_display_types(),
          ),
        );
      }

      $form['save'] = array(
        '#type' => 'submit',
        '#value' => t('Save changes'),
        '#weight' => -2,
      );
    }
  }
  elseif ($view == 'add') {
    // Get list of attributes not already assigned to this node or class.
    $unused_attributes = array();
    $result = db_query("SELECT a.aid, a.name, a.label FROM {uc_attributes} AS a LEFT JOIN {uc_attribute_options} AS ao ON a.aid = ao.aid GROUP BY a.aid, a.name, a.label ORDER BY a.name");
    foreach ($result as $attribute) {
      if (!in_array($attribute->aid, $used_aids) && !in_array($attribute->label, $used_labels)) {
        $unused_attributes[$attribute->aid] = $attribute->name;
      }
    }

    $form['add_attributes'] = array(
      '#type' => 'select',
      '#title' => t('Attributes'),
      '#description' => t('Hold Ctrl + click to select multiple attributes.'),
      '#options' => count($unused_attributes) > 0 ? $unused_attributes : array(t('No attributes left to add.')),
      '#disabled' => count($unused_attributes) == 0 ? TRUE : FALSE,
      '#multiple' => TRUE,
      '#weight' => -1
    );
    $form['add'] = array(
      '#type' => 'submit',
      '#value' => t('Add attributes'),
      '#suffix' => l(t('Cancel'), $type == 'product' ? 'node/'. $id .'/edit/attributes' : 'admin/store/products/classes/'. $class->pcid .'/attributes'),
      '#weight' => 0,
    );
  }

  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );
  $form['type'] = array(
    '#type' => 'value',
    '#value' => $type,
  );
  $form['view'] = array(
    '#type' => 'value',
    '#value' => $view,
  );

  return $form;
}

/**
 * Display the formatted attribute form.
 *
 * @ingroup themeable
 * @see uc_object_attributes_form()
 */
function theme_uc_object_attributes_form($variables) {
  $form = $variables['form'];

  $output = '';

  if ($form['view']['#value'] == 'overview') {
    $header = array(t('Remove'), t('Name'), t('Label'), t('Default'), t('Required'), t('List position'), t('Display'));

    if (count(element_children($form['attributes'])) > 0) {
      foreach (element_children($form['attributes']) as $aid) {
        $row = array(
          drupal_render($form['attributes'][$aid]['remove']),
          drupal_render($form['attributes'][$aid]['name']),
          drupal_render($form['attributes'][$aid]['label']),
          drupal_render($form['attributes'][$aid]['option']),
          drupal_render($form['attributes'][$aid]['required']),
          drupal_render($form['attributes'][$aid]['ordering']),
          drupal_render($form['attributes'][$aid]['display']),
        );

        $rows[] = array(
          'data' => $row,
          'class' => array('draggable'),
        );
      }
    }
    else {
      $rows[] = array(
        array('data' => t('You must first <a href="!url">add attributes to this !type</a>.', array('!url' => request_uri() .'/add', '!type' => $form['type']['#value'])), 'colspan' => 6),
      );
    }

    drupal_add_tabledrag('uc-attribute-table', 'order', 'sibling', 'uc-attribute-table-weight');
    $output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'uc-attribute-table')));
  }
  else {
    $output = '<div class="uc-attributes-add-link">';
    $output .= t('You may add more attributes <a href="!url">here</a>.', array('!url' => url('admin/store/attributes/add')));
    $output .= '</div>';
  }

  $output .= drupal_render_children($form);

  return $output;
}

/**
 * @see uc_object_attributes_form()
 */
function uc_object_attributes_form_submit($form, &$form_state) {
  if ($form_state['values']['type'] == 'product') {
    $attr_table = 'uc_product_attributes';
    $opt_table = 'uc_product_options';
    $id = 'nid';
  }
  elseif ($form_state['values']['type'] == 'class') {
    $attr_table = 'uc_class_attributes';
    $opt_table = 'uc_class_attribute_options';
    $id = 'pcid';
  }

  if ($form_state['values']['view'] == 'overview' && is_array($form_state['values']['attributes'])) {
    foreach ($form_state['values']['attributes'] as $aid => $attribute) {
      if ($attribute['remove']) {
        $remove_aids[] = $aid;
      }
      else {
        $attribute['aid'] = $aid;
        $attribute[$id] = $form_state['values']['id'];
        drupal_write_record($attr_table, $attribute, array('aid', $id));
        $changed = TRUE;
      }
    }

    if (count($remove_aids) > 0) {
      $id_value = $form_state['values']['id'];

      // Can't use "{$opt_table}" because PHP will remove the braces, which
      // prevents the db_prefix from being appended.
      $select = db_select('uc_attribute_options', 'ao')
        ->where('{' . $opt_table . '}.oid = ao.oid')
        ->condition('ao.oid', $remove_aids, 'IN');
      $select->addExpression('1');
      db_delete($opt_table)
        ->condition('', $select, 'EXISTS')
        ->condition('{' . $opt_table . "}.$id", $id_value)
        ->execute();

      db_delete($attr_table)
        ->condition($id, $id_value)
        ->condition('aid', $remove_aids, 'IN')
        ->execute();

      if ($form_state['values']['type'] == 'product') {
        db_delete('uc_product_adjustments')
          ->condition('nid', $id_value)
          ->execute();
      }

      drupal_set_message(format_plural(count($remove_aids), '@count attribute has been removed.', '@count attributes have been removed.'));
    }

    if ($changed) {
      drupal_set_message(t('The changes have been saved.'));
    }
  }
  elseif ($form_state['values']['view'] == 'add') {
    foreach ($form_state['values']['add_attributes'] as $aid) {
      // Enable all options for added attributes.
      $attribute = uc_attribute_load($aid);
      foreach ($attribute->options as $option) {
        $option->aid = $id;
        db_insert($opt_table)
          ->fields((array) $option)
          ->execute();
      }
      // Make the first option (if any) the default.
      $option = reset($attribute->options);
      if ($option) {
        $oid = $option->oid;
      }
      else {
        $oid = 0;
      }

      $select = db_select('uc_attributes', 'a')
        ->condition('aid', $aid);
      $select->addExpression(':id', $id, array(':id' => $form_state['values']['id']));
      $select->addField('a', 'aid');
      $select->addField('a', 'label');
      $select->addField('a', 'ordering');
      $select->addExpression(':id', 'default_option', array(':id' => $oid));
      $select->addField('a', 'required');
      $select->addField('a', 'display');

      db_insert($attr_table)
        ->from($select)
        ->execute();
    }
    if (count($form_state['values']['add_attributes']) > 0) {
      if ($form_state['values']['type'] == 'product') {
        db_delete('uc_product_adjustments')
          ->condition('nid', $form_state['values']['id'])
          ->execute();
      }
      drupal_set_message(format_plural(count($form_state['values']['add_attributes']), '@count attribute has been added.', '@count attributes have been added.'));
    }
  }

  if ($form_state['values']['type'] == 'product') {
    $form_state['redirect'] = 'node/'. $form_state['values']['id'] .'/edit/attributes';
  }
  else {
    $form_state['redirect'] = 'admin/store/products/classes/'. $form_state['values']['id'] .'/attributes';
  }
}

/**
 * Form to assign and modify attribute options on products or classes.
 *
 * @ingroup forms
 * @see
 *   theme_uc_object_options_form()
 *   uc_object_options_form_validate()
 *   uc_object_options_form_submit()
 */
function uc_object_options_form($form, &$form_state, $object, $type) {
  if ($type == 'product') {
    $product = $object;
    $id = $product->nid;
    drupal_set_title(check_plain($product->title));
    $attributes = uc_product_get_attributes($id);
    $table = 'uc_product_options';
    $id_type = 'nid';
  }
  elseif ($type == 'class') {
    $class = $object;
    $id = $class->pcid;
    drupal_set_title(check_plain($class->name));
    $attributes = uc_class_get_attributes($id);
    $table = 'uc_class_attribute_options';
    $id_type = 'pcid';
  }

  foreach ($attributes as $aid => $attribute) {
    $form['attributes'][$aid]['name'] = array(
      '#markup' => check_plain($attribute->name),
    );
    $form['attributes'][$aid]['aid'] = array(
      '#type' => 'hidden',
      '#value' => $attribute->aid,
    );
    $form['attributes'][$aid]['ordering'] = array(
      '#type' => 'value',
      '#value' => $attribute->ordering,
    );

    $form['attributes'][$aid]['options'] = array('#weight' => 2);

    $base_attr = uc_attribute_load($attribute->aid);

    if ($base_attr->options) {
      $options = array();

      $query = db_select('uc_attribute_options', 'ao');
      $query->leftJoin($table, 'po', "ao.oid = po.oid AND po.$id_type = :id", array(':id' => $id))
        ->fields('ao', array(
          'aid',
          'oid',
          'name',
        ));

      $query->addField('ao', 'cost', 'default_cost');
      $query->addField('ao', 'price', 'default_price');
      $query->addField('ao', 'weight', 'default_weight');
      $query->addField('ao', 'ordering', 'default_ordering');

      $query->fields('po', array(
          'cost',
          'price',
          'weight',
          'ordering',
        ))
        ->addExpression('po.ordering IS NULL', 'null_order');

      $query->condition('aid', $attribute->aid)
        ->orderBy('null_order')
        ->orderBy('po.ordering')
        ->orderBy('default_ordering')
        ->orderBy('ao.name');

      $result = $query->execute();
      foreach ($result as $option) {
        $oid = $option->oid;
        $options[$oid] = '';

        $form['attributes'][$aid]['options'][$oid]['select'] = array(
          '#type' => 'checkbox',
          '#default_value' => isset($attribute->options[$oid]) ? TRUE : FALSE,
          '#title' => check_plain($option->name),
        );
        $form['attributes'][$aid]['options'][$oid]['cost'] = array(
          '#type' => 'textfield',
          '#default_value' => uc_store_format_price_field_value(is_null($option->cost) ? $option->default_cost : $option->cost),
          '#size' => 6,
        );
        $form['attributes'][$aid]['options'][$oid]['price'] = array(
          '#type' => 'textfield',
          '#default_value' => uc_store_format_price_field_value(is_null($option->price) ? $option->default_price : $option->price),
          '#size' => 6,
        );
        $form['attributes'][$aid]['options'][$oid]['weight'] = array(
          '#type' => 'textfield',
          '#default_value' => is_null($option->weight) ? $option->default_weight : $option->weight,
          '#size' => 5,
        );
        $form['attributes'][$aid]['options'][$oid]['ordering'] = array(
          '#type' => 'weight',
          '#delta' => 50,
          '#default_value' => is_null($option->ordering) ? $option->default_ordering : $option->ordering,
          '#attributes' => array('class' => array('uc-attribute-option-table-ordering')),
        );
      }

      $form['attributes'][$aid]['default'] = array(
        '#type' => 'radios',
        '#options' => $options,
        '#default_value' => /* $attribute->required ? NULL : */ $attribute->default_option,
        //'#disabled' => $attribute->required,
      );
    }
    else {
      $form['attributes'][$aid]['default'] = array(
        '#markup' => t('This attribute does not have any options.'),
      );
    }
  }

  if (!empty($form['attributes'])) {
    $form['attributes']['#tree'] = TRUE;

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
      '#weight' => 10,
    );
  }

  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );
  $form['type'] = array(
    '#type' => 'value',
    '#value' => $type,
  );

  return $form;
}

/**
 * Display the option form.
 *
 * @ingroup themeable
 * @see uc_object_options_form()
 */
function theme_uc_object_options_form($variables) {
  $form = $variables['form'];

  $header = array(array('data' => '&nbsp;&nbsp;'. t('Options')) + theme('table_select_header_cell'), t('Default'), t('Cost'), t('Price'), t('Weight'), t('List position'));
  $table_id_num = $tables = 0;

  foreach (element_children($form['attributes']) as $key) {

    $rows = array();

    if (element_children($form['attributes'][$key]['default'])) {

      foreach (element_children($form['attributes'][$key]['default']) as $oid) {
        $row = array(
          drupal_render($form['attributes'][$key]['options'][$oid]['select']),
          drupal_render($form['attributes'][$key]['default'][$oid]),
          drupal_render($form['attributes'][$key]['options'][$oid]['cost']),
          drupal_render($form['attributes'][$key]['options'][$oid]['price']),
          drupal_render($form['attributes'][$key]['options'][$oid]['weight']),
          drupal_render($form['attributes'][$key]['options'][$oid]['ordering']),
        );

        $rows[] = array(
          'data' => $row,
          'class' => array('draggable'),
        );
      }
      $table_id = 'uc-attribute-option-table-'. $table_id_num++;
      drupal_add_tabledrag($table_id, 'order', 'sibling', 'uc-attribute-option-table-ordering');
    }
    else {
      $row = array();
      $row[] = array('data' => drupal_render($form['attributes'][$key]['default']), 'colspan' => 6);
      $rows[] = $row;
    }

    if (!count($rows)) {
      $rows[] = array(array('data' => t('This !type does not have any attributes.', array('!type' => $form['type']['#value'] == 'product' ? t('product') : t('product class'))), 'colspan' => 6));
    }


    $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('product_attributes'), 'id' => $table_id), 'caption' => '<h2>'. drupal_render($form['attributes'][$key]['name']) .'</h2>'));

    $tables++;
  }

  if (!$tables) {

    $output .= '<br /><br />';
    if ($form['type']['#value'] == 'product') {
      drupal_set_message(t('This product does not have any attributes.'), 'warning');
    }
    else {
      drupal_set_message(t('This product class does not have any attributes.'), 'warning');
    }
  }

  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Returns a themed set of attribute options for use in order displays.
 *
 * @param $element
 *   Structured array containing the set of attributes with each element
 *   having a key of attribute ID and the following keys:
 *     #attribute_name - Attribute name
 *     #options - Array of option names
 * @return Themed set of attribute options.
 */
function theme_uc_product_attributes($variables) {
  $attributes = $variable['attributes'];

  $option_rows = array();

  foreach (element_children($attributes) as $key) {
    $optionstr = '';

    foreach ((array)$attributes[$key]['#options'] as $option) {
      // We only need to allow translation from the second option onward
      if (empty($optionstr)) {
        $optionstr .= $option;
      }
      else {
        $optionstr .= t(', !option', array('!option' => $option));
      }
    }
    $option_rows[$key] = t('@attribute: @option', array('@attribute' => $attributes[$key]['#attribute_name'], '@option' => $optionstr));
  }

  if (!empty($option_rows)) {
    return theme('item_list', array('items' => array_values($option_rows), 'attributes' => array('class' => array('product-description'))));
  }

  return '';
}

/**
 * Make sure that all selected default options are enabled.
 *
 * @see uc_object_options_form()
 */
function uc_object_options_form_validate($form, &$form_state) {
  if (isset($form_state['values']['attributes'])) {
    foreach ($form_state['values']['attributes'] as $aid => $attribute) {
      $selected_opts = array();
      if (is_array($attribute['options'])) {
        foreach ($attribute['options'] as $oid => $option) {
          if ($option['select'] == 1) {
            $selected_opts[] = $oid;
          }
        }
      }
      if (!empty($selected_opts) && !$form['attributes'][$aid]['default']['#disabled'] && !in_array($attribute['default'], $selected_opts)) {
        form_set_error($attribute['default']);
        $error = TRUE;
      }
    }
  }

  if ($error) {
    drupal_set_message(t('All attributes with enabled options must specify an enabled option as default.'), 'error');
  }
}

/**
 * @see uc_object_options_form()
 */
function uc_object_options_form_submit($form, &$form_state) {
  if ($form_state['values']['type'] == 'product') {
    $attr_table = 'uc_product_attributes';
    $opt_table = 'uc_product_options';
    $id = 'nid';
  }
  elseif ($form_state['values']['type'] == 'class') {
    $attr_table = 'uc_class_attributes';
    $opt_table = 'uc_class_attribute_options';
    $id = 'pcid';
  }

  foreach ($form_state['values']['attributes'] as $attribute) {
    db_update($attr_table)
      ->fields(array(
        'default_option' => $attribute['default'],
      ))
      ->condition($id, $form_state['values']['id'])
      ->condition('aid', $attribute['aid'])
      ->execute();

    if (is_array($attribute['options'])) {
      db_delete($opt_table)
        ->condition($id, $form_state['values']['id'])
        ->condition('oid', array_keys($attribute['options']), 'IN')
        ->execute();

      foreach ($attribute['options'] as $oid => $option) {
        if ($option['select']) {
          $option[$id] = $form_state['values']['id'];
          $option['oid'] = $oid;

          drupal_write_record($opt_table, $option);
        }
        elseif ($form_state['values']['type'] == 'product') {
          $aid = $attribute['aid'];
          $match = 'i:'. $aid .';s:'. strlen($oid) .':"'. $oid .'";';
          db_delete('uc_product_adjustments')
            ->condition('nid', $form_state['values']['id'])
            ->condition('combination', '%' . db_like($match) . '%', 'LIKE')
            ->execute();
        }
      }
    }
  }

  drupal_set_message(t('The !type options have been saved.', array('!type' => $form_state['values']['type'] == 'product' ? t('product') : t('product class'))));
}

/**
 * Form builder to associate option combinations with mutations of a product's model number.
 *
 * @ingroup forms
 * @see uc_product_adjustments_form_submit()
 */
function uc_product_adjustments_form($form, &$form_state, $node) {
  drupal_set_title(check_plain($node->title));
  $nid = $node->nid;

  //Populate table and such.
  $model = $node->model;
  $query_select = "SELECT DISTINCT";
  $query_from = " FROM";
  $query_where = " WHERE";
  $query_order = " ORDER BY";
  $result = db_query("SELECT pa.nid, pa.aid, pa.ordering, pa.display, a.name, a.ordering, ao.aid, COUNT(po.oid) FROM {uc_product_attributes} AS pa LEFT JOIN {uc_attributes} AS a ON pa.aid = a.aid LEFT JOIN {uc_attribute_options} AS ao ON a.aid = ao.aid LEFT JOIN {uc_product_options} AS po ON ao.oid = po.oid AND po.nid = :po_nid WHERE pa.nid = :pa_nid AND pa.display <> :pa_display GROUP BY ao.aid, pa.aid, pa.display, a.name, pa.ordering, a.ordering, pa.nid HAVING count(po.oid) > 0 ORDER BY pa.ordering, a.ordering", array(':po_nid' => $nid, ':pa_nid' => $nid, ':pa_display' => 3));
  $i = 1;
  $attribute_names = '';
  $full_attributes = array();
  $values = array();

  $query = db_select('uc_product_options', "po$i")->extend('PagerDefault')
    ->limit(20);

  foreach ($result as $prod_attr) {
    if ($i > 1) {
      $query->join('uc_product_options', "po$i");
    }
    $query->leftJoin('uc_attribute_options', "ao$i", "po$i.oid = ao$i.oid AND po$i.nid = :nid", array(':nid' => $nid));
    $query->addField("ao$i", 'aid', "aid$i");
    $query->addField("ao$i", 'name', "name$i");
    $query->addField("ao$i", 'oid', "oid$i");
    $query->addField("po$i", 'ordering', "ordering$i");

    $query->condition("ao$i.aid", $prod_attr->aid)
      ->orderBy("po$i.ordering")
      ->orderBy("ao$i.name");

    ++$i;
    $attribute_names .= '<th>'. check_plain($prod_attr->name) .'</th>';
    $attribute_ids[] = $prod_attr->aid;
  }
  $num_prod_attr = count($attribute_ids);

  if ($num_prod_attr) {
    //Get previous values
    $old_val = db_query("SELECT * FROM {uc_product_adjustments} WHERE nid = :nid", array(':nid' => $nid))->fetchAll();

    $result = $query->execute();

    $form['original'] = array(
      '#markup' => '<div><br /><b>'. t('Default product SKU: @sku', array('@sku' => $model)) .'</b></div>',
    );
    $form['default'] = array(
      '#type' => 'value',
      '#value' => $model,
    );
    $form['table'] = array(
      '#prefix' => '<table class="combinations">',
      '#suffix' => '</table>',
    );
    $form['table']['head'] = array(
      '#markup' => '<thead><tr>' . $attribute_names .'<th>Alternate SKU</th></tr></thead>',
      '#weight' => 0,
    );
    $form['table']['body'] = array(
      '#prefix' => '<tbody>',
      '#suffix' => '</tbody>',
      '#weight' => 1,
      '#tree' => TRUE,
    );

    $i = 0;
    while ($combo = $result->fetchObject()) {
      $cells = '';
      $row_title = '';
      $comb_array = array();
      for ($j = 1; $j <= $num_prod_attr; ++$j) {
        $cells .= '<td>'. check_plain($combo->{'name'. $j}) .'</td>';
        $row_title .= check_plain($combo->{'name'. $j}) .', ';
        $comb_array[$combo->{'aid'. $j}] = $combo->{'oid'. $j};
      }
      ksort($comb_array);
      $row_title = substr($row_title, 0, strlen($row_title) - 2);
      $default_model = $model;
      foreach ($old_vals as $ov) {
        if (!count(array_diff_assoc(unserialize($ov->combination), $comb_array))) {
          $default_model = $ov->model;
          break;
        }
      }

      $form['table']['body'][$i] = array(
        '#prefix' => '<tr title="'. $row_title .'">',
        '#suffix' => '</tr>',
      );
      $form['table']['body'][$i]['combo'] = array(
        '#markup' => $cells,
      );
      $form['table']['body'][$i]['combo_array'] = array(
        '#type' => 'value',
        '#value' => serialize($comb_array),
      );
      $form['table']['body'][$i]['model'] = array(
        '#type' => 'textfield',
        '#default_value' => $default_model,
        '#prefix' => '<td>',
        '#suffix' => '</td>',
      );
      ++$i;
    }

    $form['nid'] = array(
      '#type' => 'hidden',
      '#value' => $nid,
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
    );
  }
  else {
    $form['error'] = array(
      '#markup' => '<div><br />'. t('This product does not have any attributes.') .'</div>',
    );
  }

  $form['pager'] = array(
    '#theme' => 'pager',
  );

  return $form;
}

/**
 * @see uc_product_adjustments_form()
 */
function uc_product_adjustments_form_submit($form, &$form_state) {
  foreach ($form_state['values']['body'] as $value) {
    if (!empty($value['model']) && $value['model'] != $form_state['values']['default']) {
      db_merge('uc_product_adjustments')
        ->key(array(
          'nid' => $form_state['values']['nid'],
          'combination' => $value['combo_array'],
        ))
        ->fields(array(
          'model' => $value['model'],
        ))
        ->execute();
    }
    else {
      db_delete('uc_product_adjustments')
        ->condition('nid', $form_state['values']['nid'])
        ->condition('combination', $value['combo_array'])
        ->execute();
    }
  }
  drupal_set_message(t('Product adjustments have been saved.'));
  $goto = array($_GET['q']);
  if ($_GET['page']) {
    $goto[] = 'page='. $_GET['page'];
  }
  $form_state['redirect'] = $goto;
}

