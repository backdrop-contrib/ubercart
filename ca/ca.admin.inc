<?php
// $Id$

/**
 * @file
 * Conditional actions overview UI.
 */

// Displays the administration page that lets you add and modify predicates.
function ca_admin() {
  // Load all the module defined predicates into a temporary array.
  $temp = module_invoke_all('ca_predicate');

  // Assign a default weight if need be.
  foreach ($temp as $key => $value) {
    $temp[$key]['#pid'] = $key;
    if (!isset($value['#weight'])) {
      $temp[$key]['#weight'] = 0;
    }
  }

  // Load and loop through all the database stored predicates.
  $result = db_query("SELECT * FROM {ca_predicates}");
  while ($predicate = db_fetch_array($result)) {
    // Prepare the database data into a predicate.
    $predicate = ca_prepare_db_predicate($predicate);

    // Overrides the module defined predicate if it's been modified by a user.
    $temp[$predicate['#pid']] = $predicate;
  }

  usort($temp, 'ca_weight_sort');

  // Copy the temporary array of predicates into a keyed array.
  $predicates = array();
  foreach ($temp as $predicate) {
    $predicates[$predicate['#pid']] = $predicate;
  }

  // Load up the trigger data so we can display them by title.
  $triggers = module_invoke_all('ca_trigger');

  // Build the header for the predicate tables.
  $header = array(t('Title'), t('Class'), t('Status'), t('Weight'), t('Operations'));

  $rows = array();

  foreach ($predicates as $key => $value) {
    // Build the basic operation links for each predicate.
    $ops = array(
      l(t('edit'), 'admin/settings/ca/'. $key .'/edit'),
    );

    // Add the reset link if a module defined predicate has been modified.
    if (!is_numeric($key) && isset($value['#modified'])) {
      $ops[] = l(t('reset'), 'admin/settings/ca/'. $key .'/reset');
    }

    // Add a delete link if displaying a custom predicate.
    if (is_numeric($key)) {
      $ops[] = l(t('delete'), 'admin/settings/ca/'. $key .'/delete');
    }

    // Add the predicate's row to the table.
    $tables[$triggers[$value['#trigger']]['#title']]['rows'][] = array(
      check_plain($value['#title']),
      strpos($value['#class'], 'custom') === 0 ? check_plain(substr($value['#class'], 7)) : $value['#class'],
      $value['#status'] == 0 ? t('Disabled') : t('Enabled'),
      $value['#weight'],
      implode(' ', $ops),
    );
  }

  // Add the tables for each trigger to the output.
  foreach ($tables as $key => $value) {
    $output .= theme('table', $header, $value['rows'], array(), '<strong>'. t('Trigger') .':</strong> '. check_plain($key));
  }

  $output .= l(t('Add a predicate'), 'admin/settings/ca/add');

  return $output;
}

// Form to allow the creation and editing of conditional action predicates.
function ca_predicate_meta_form($form_state, $pid) {
  // Load the predicate if an ID is passed in.
  if (!empty($pid) && $pid !== 0) {
    $predicate = ca_load_predicate($pid);

    // Fail out if we didn't find a predicate for that ID.
    if (empty($predicate)) {
      drupal_set_message(t('That predicate does not exist.'), 'error');
      drupal_goto('admin/settings/ca');
    }
  }

  $form['predicate_pid'] = array(
    '#type' => 'value',
    '#value' => $pid,
  );

  $form['predicate_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Enter a title used for display on the overview tables.'),
    '#default_value' => $pid ? $predicate['#title'] : '',
    '#required' => TRUE,
  );

  // Create an array of triggers for the select box.
  $triggers = module_invoke_all('ca_trigger');
  foreach ($triggers as $key => $value) {
    $options[$value['#category']][$key] = $value['#title'];
  }

  $form['predicate_trigger'] = array(
    '#type' => 'select',
    '#title' => t('Trigger'),
    '#description' => t('Select the trigger for this predicate.<br />Cannot be modified if the predicate has conditions or actions.'),
    '#options' => $options,
    '#default_value' => $pid ? $predicate['#trigger'] : '',
    '#disabled' => empty($predicate['#conditions']) && empty($predicate['#actions']) ? FALSE : TRUE,
    '#required' => TRUE,
  );

  $form['predicate_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Enter a description that summarizes the use and intent of the predicate.'),
    '#default_value' => $pid ? $predicate['#description'] : '',
  );

  // Accommodate the mandatory custom prefix for predicate classes.
  $class = $predicate['#class'];
  if (strpos($class, 'custom') === 0) {
    $class = ltrim(substr($class, 6), ':');
  }

  $form['predicate_class'] = array(
    '#type' => 'textfield',
    '#title' => t('Class'),
    '#description' => t('Classes let you categorize your predicates based on the type of functionality they provide.<br />Cannot be modified if defined by a module.'),
    '#field_prefix' => is_numeric($pid)  ? 'custom:' : '',
    '#default_value' => $class,
    '#disabled' => !is_numeric($pid) ? TRUE : FALSE,
  );

  $form['predicate_status'] = array(
    '#type' => 'radios',
    '#title' => t('Status'),
    '#description' => t('Disabled predicates will not be processed when their trigger is pulled.'),
    '#options' => array(
      1 => t('Enabled'),
      0 => t('Disabled'),
    ),
    '#default_value' => $pid ? $predicate['#status'] : 1,
  );

  $form['predicate_weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#description' => t('Predicates will be sorted by weight and processed sequentially.'),
    '#default_value' => ($pid && isset($predicate['#weight'])) ? $predicate['#weight'] : 0,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save predicate'),
    '#suffix' => l(t('Cancel'), 'admin/settings/ca'),
  );

  return $form;
}

function ca_predicate_meta_form_submit($form, &$form_state) {
  $save = FALSE;

  // Load the original predicate.
  if ($form_state['values']['pid'] !== 0) {
    $predicate = ca_load_predicate($form_state['values']['predicate_pid']);
    $predicate['#pid'] = $form_state['values']['predicate_pid'];
  }

  // Setup a list of fields to check for and apply changes.
  $fields = array('title', 'trigger', 'description', 'class', 'status', 'weight');

  // Compare the values from the form submission with what is already set.
  foreach ($fields as $field) {
    if ($form_state['values']['predicate_'. $field] != $predicate['#'. $field]) {
      $predicate['#'. $field] = $form_state['values']['predicate_'. $field];
      $save = TRUE;
    }
  }

  // Add empty conditions and actions arrays if this is a new predicate.
  if (empty($predicate['#pid'])) {
    $predicate['#pid'] = db_next_id('{ca_predicates}_pid');
    $predicate['#conditions'] = array();
    $predicate['#actions'] = array();
  }

  // Check to see if any changes were made and save if necessary.
  if ($save) {
    ca_save_predicate($predicate);
  }

  drupal_set_message(t('Predicate meta data saved.'));

  $form_state['redirect'] = 'admin/settings/ca';
}

// Form to reset a modified module defined predicate to its original state.
function ca_predicate_delete_form($form_state, $pid) {
  $predicate = ca_load_predicate($pid);

  // Fail if we received an invalid predicate ID.
  if (empty($predicate)) {
    drupal_set_message(t('That predicate does not exist.'), 'error');
    drupal_goto('admin/settings/ca');
  }

  $form['predicate_pid'] = array(
    '#type' => 'value',
    '#value' => $pid,
  );

  $form['predicate_title'] = array(
    '#type' => 'value',
    '#value' => $predicate['#title'],
  );

  $description = '<p><strong>'. check_plain($predicate['#title']) .'</strong><br />'
               . check_plain($predicate['#description']) .'</p><p>'
                . t('This action cannot be undone.') .'</p>';

  return confirm_form($form, t('Are you sure you want to !op this predicate?', array('!op' => is_numeric($pid) ? t('delete') : t('reset'))), 'admin/settings/ca', $description);
}

function ca_predicate_delete_form_submit($form_id, &$form_state) {
  ca_delete_predicate($form_state['values']['predicate_pid']);

  drupal_set_message(t('Predicate %title !op.', array('%title' => $form_state['values']['predicate_title'], '!op' => is_numeric($form_state['values']['predicate_pid']) ? t('deleted') : t('reset'))));

  $form_state['redirect'] = 'admin/settings/ca';
}

