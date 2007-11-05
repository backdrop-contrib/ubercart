<?php
// $Id$

/**
 * @file
 * This file contains the Workflow-ng hooks and functions necessary to make the
 * tax related entity, conditions, events, and actions work.
 */

/******************************************************************************
 * Workflow-ng Hooks                                                          *
 ******************************************************************************/

/**
 * Implementation of hook_entity_info().
 */
function uc_taxes_entity_info(){
  $entities['line_item'] = array(
    '#save' => 'uc_taxes_save_line_item',
  );
  return $entities;
}

/**
 * Implementation of hook_event_info().
 *
 * Register an event for each tax rule in {uc_taxes}.
 */
function uc_taxes_event_info(){
  $events = array();
  $taxes = uc_taxes_get_rates();
  foreach ($taxes as $tax){
    $events['calculate_tax_'. $tax->id] = array(
      '#label' => t('Calculate @name', array('@name' => $tax->name)),
      '#module' => 'uc_taxes',
      '#arguments' => array(
        'order' => array('#entity' => 'order', '#label' => t('Order')),
        'tax' => array('#entity' => 'tax', '#label' => t('Tax')),
        'line_item' => array('#entity' => 'line_item', '#label' => t('Line Item')),
      ),
    );
  }
  return $events;
}

/**
 * Implementation of hook_action_info().
 */
function uc_taxes_action_info(){
  $actions = array();
  $actions['uc_taxes_action_apply_tax'] = array(
    '#label' => t('Charge a tax'),
    '#arguments' => array(
      'order' => array('#entity' => 'order', '#label' => t('Order')),
      'tax' => array('#entity' => 'tax', '#label' => t('Tax')),
      'line_item' => array('#entity' => 'line_item', '#label' => t('Line Item')),
    ),
    '#module' => t('Taxes'),
  );
  return $actions;
}

/**
 * Workflow-ng action callback to calculate a tax.
 *
 * @param $order
 *   The order object being considered.
 * @param $tax
 *   The tax rule calculating the amount.
 * @return
 *   The line item array representing the amount of tax.
 */
function uc_taxes_action_apply_tax($order, $tax){
  $amount = 0;
  if (is_array($order->products)) {
    foreach($order->products as $item){
      $node = node_load($item->nid);
      if (in_array($node->type, $tax->taxed_product_types)){
        $amount += $item->price * $item->qty * $tax->rate;
      }
    }
  }
  $taxed_line_items = $tax->taxed_line_items;
  if (is_array($order->line_items) && is_array($taxed_line_items)){
    foreach ($order->line_items as $key => $line_item){
      if (in_array($line_item['type'], $taxed_line_items)){
        $amount += $line_item['amount'] * $tax->rate;
      }
    }
  }
  if ($amount){
    $line_item = (object)array('id' => $tax->id, 'name' => $tax->name, 'amount' => $amount, 'weight' => $tax->weight);
    return array('line_item' => $line_item);
  }
}

/**
 * Implementation of hook_configuration().
 *
 * Create a configuration for each event corresponding to a tax rule.
 */
function uc_taxes_configuration(){
  $configurations = array();
  
  $taxes = uc_taxes_get_rates();
  foreach ($taxes as $tax){
    $configurations['uc_taxes_'. $tax->id] = array(
      '#label' => $tax->name,
      '#event' => 'calculate_tax_'. $tax->id,
      '#module' => 'uc_taxes',
      '#active' => true,
    );
    $action = workflow_ng_use_action('uc_taxes_action_apply_tax', array(
      '#label' => t('Apply @tax', array('@tax' => $tax->name)),
    ));
    $configurations['uc_taxes_'. $tax->id] = workflow_ng_configure($configurations['uc_taxes_'. $tax->id], $action);
  }

  return $configurations;
}

/**
 * Preserve each tax line item for future use.
 */
function uc_taxes_save_line_item($line_item){
  $_SESSION['taxes'][$line_item->id] = array('id' => $line_item->id, 'name' => $line_item->name, 'amount' => $line_item->amount, 'weight' => $line_item->weight);
}
