<?php
// $Id$

/**
 * @file
 * This file contains the Workflow-ng hooks and functions necessary to make the
 * order related entity, conditions, events, and actions work.
 */


/******************************************************************************
 * Workflow-ng Hooks                                                          *
 ******************************************************************************/

// Define the order entity for use in Workflow.
function uc_order_entity_info(){
  $entities['order'] = array(
    '#save' => 'uc_order_save',
  );

  return $entities;
}

// Tell Workflow about the various order events.
function uc_order_event_info(){
  $events['order_status_update'] = array(
    '#label' => t('Order status gets updated'),
    '#module' => t('Order'),
    '#arguments' => array(
      'order' => array('#entity' => 'order', '#label' => t('Order')),
      'updated_order' => array('#entity' => 'order', '#label' => t('Updated order')),
    ),
  );

  return $events;
}

// Tell Workflow about the various order conditions.
function uc_order_condition_info(){
  $order_arg = array('#entity' => 'order', '#label' => t('Order'));

  $conditions['uc_order_condition_order_status'] = array(
    '#label' => t('Check the order status'),
    '#arguments' => array('order' => $order_arg),
    '#module' => t('Order'),
  );
  $conditions['uc_order_condition_delivery_postal_code'] = array(
    '#label' => t("Check an order's delivery postal code"),
    '#arguments' => array('order' => $order_arg),
    '#description' => t('Returns TRUE if the delivery address is in the specified area.'),
    '#module' => t('Order: Delivery address'),
  );
  $conditions['uc_order_condition_delivery_zone'] = array(
    '#label' => t("Check an order's delivery zone"),
    '#arguments' => array('order' => $order_arg),
    '#description' => t('Returns TRUE if the delivery zone is in the specified list.'),
    '#module' => t('Order: Delivery address'),
  );
  $conditions['uc_order_condition_delivery_country'] = array(
    '#label' => t("Check an order's delivery country"),
    '#arguments' => array('order' => $order_arg),
    '#description' => t('Returns TRUE if the delivery country is in the specified list.'),
    '#module' => t('Order: Delivery address'),
  );

  return $conditions;
}

// Tell Workflow about the various order actions.
function uc_order_action_info(){
  $order_arg = array('#entity' => 'order', '#label' => t('Order'));

  $actions['uc_order_action_update_status'] = array(
    '#label' => t('Update the order status'),
    '#arguments' => array('order' => $order_arg),
    '#module' => t('Order'),
  );

  return $actions;
}


/******************************************************************************
 * Workflow-ng Condition Callbacks and Forms                                  *
 ******************************************************************************/

// Check the current order status.
function uc_order_condition_order_status($order, $settings) {
  // Return TRUE if the order status matches.
  if ($order->order_status == $settings['order_status']) {
    return TRUE;
  }
  return FALSE;
}

function uc_order_condition_order_status_form($settings = array()) {
  foreach (uc_order_status_list('general') as $status) {
    $options[$status['id']] = $status['title'];
  }
  foreach (uc_order_status_list('specific') as $status) {
    $options[$status['id']] = $status['title'];
  }
  $form['order_status'] = array(
    '#type' => 'select',
    '#title' => t('Order status'),
    '#options' => $options,
    '#default_value' => $settings['order_status'],
  );

  return $form;
}

function uc_order_condition_order_status_submit($form_id, $form_values){
  return array('order_status' => $form_values['order_status']);
}

// Check an order's delivery postal code.
function uc_order_condition_delivery_postal_code($order, $settings) {
  // Trim the wildcard off the pattern.
  $pattern = rtrim($settings['pattern'], '*');

  // Return TRUE if the delivery postal code begins with the pattern.
  return strpos($order->delivery_postal_code, $pattern) === 0;
}

function uc_order_condition_delivery_postal_code_form($settings = array()){
  $form['pattern'] = array(
    '#type' => 'textfield',
    '#title' => uc_get_field_name('postal_code'),
    '#default_value' => $settings['pattern'],
    '#description' => t('Specify a postal code or postal code pattern. Use "*" as a wild card to specify a range of postal codes.<br /><b>Example:</b> In the US, 402* represents all areas from 40200 to 40299.'),
    '#size' => 15,
    '#required' => TRUE,
  );

  return $form;
}

function uc_order_condition_delivery_postal_code_submit($form_id, $form_values){
  return array('pattern' => $form_values['pattern']);
}

// Check an order's delivery zone.
function uc_order_condition_delivery_zone($order, $settings){
  return in_array($order->delivery_zone, $settings['zones']);
}

function uc_order_condition_delivery_zone_form($settings = array()){
  $result = db_query("SELECT z.*, c.country_name FROM {uc_zones} AS z LEFT JOIN {uc_countries} AS c ON z.zone_country_id = c.country_id ORDER BY c.country_name, z.zone_name");
  while ($zone = db_fetch_object($result)) {
    $options[$zone->country_name][$zone->zone_id] = $zone->zone_name;
  }

  $form['zones'] = array(
    '#type' => 'select',
    '#title' => uc_get_field_name('zone'),
    '#options' => $options,
    '#default_value' => $settings['zones'],
    '#multiple' => TRUE,
    '#required' => TRUE,
  );
  
  return $form;
}

function uc_order_condition_delivery_zone_submit($form_id, $form_values){
  return array('zones' => $form_values['zones']);
}

// Check an order's delivery country.
function uc_order_condition_delivery_country($order, $settings){
  return in_array($order->delivery_country, $settings['countries']);
}

function uc_order_condition_delivery_country_form($settings = array()){
  $form['countries'] = uc_country_select(uc_get_field_name('country'), $settings['countries']);
  $form['countries']['#multiple'] = TRUE;
  $form['countries']['#required'] = TRUE;
  
  return $form;
}

function uc_order_condition_delivery_country_submit($form_id, $form_values){
  return array('countries' => $form_values['countries']);
}


/******************************************************************************
 * Workflow-ng Action Callbacks and Forms                                     *
 ******************************************************************************/

// Update an order's status.
function uc_order_action_update_status($order, $settings) {
  uc_order_update_status($order->order_id, $settings['order_status']);
}

function uc_order_action_update_status_form($settings = array()) {
  foreach (uc_order_status_list('general') as $status) {
    $options[$status['id']] = $status['title'];
  }
  foreach (uc_order_status_list('specific') as $status) {
    $options[$status['id']] = $status['title'];
  }
  $form['order_status'] = array(
    '#type' => 'select',
    '#title' => t('Order status'),
    '#options' => $options,
    '#default_value' => $settings['order_status'],
  );

  return $form;
}

function uc_order_action_update_status_submit($form_id, $form_values){
  return array('order_status' => $form_values['order_status']);
}

