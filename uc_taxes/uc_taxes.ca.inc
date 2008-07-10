<?php
// $Id$

/**
 * @file
 * This file contains the Conditional Action hooks and functions necessary to
 * make the tax related entity, conditions, events, and actions work.
 */

/******************************************************************************
 * Conditional Action Hooks                                                   *
 ******************************************************************************/

/**
 * Implementation of hook_ca_entity().
 */
function uc_taxes_ca_entity() {
  $entities['tax'] = array(
    '#title' => t('Tax rule'),
    '#type' => 'object',
  );
  return $entities;
}

/**
 * Implementation of hook_ca_trigger().
 *
 * Register an event for each tax rule in {uc_taxes}.
 */
function uc_taxes_ca_trigger() {
  $triggers = array();
  $taxes = uc_taxes_get_rates();
  foreach ($taxes as $tax) {
    $triggers['calculate_tax_'. $tax->id] = array(
      '#title' => t('Calculate @name', array('@name' => $tax->name)),
      '#category' => 'uc_taxes',
      '#arguments' => array(
        'order' => array('#entity' => 'order', '#title' => t('Order')),
        'tax' => array('#entity' => 'tax', '#title' => t('Tax rule')),
        'user' => array('#entity' => 'user', '#title' => t('User account')),
      ),
    );
  }
  return $triggers;
}

/**
 * Implementation of hook_action_info().
 */
function uc_taxes_ca_action() {
  $actions = array();
  $actions['uc_taxes_action_apply_tax'] = array(
    '#title' => t('Charge a tax'),
    '#category' => t('Taxes'),
    '#arguments' => array(
      'order' => array('#entity' => 'order', '#title' => t('Order')),
      'tax' => array('#entity' => 'tax', '#title' => t('Tax')),
    ),
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
function uc_taxes_action_apply_tax($order, $tax) {
  $amount = 0;
  if (is_array($order->products)) {
    foreach ($order->products as $item) {
      $node = node_load($item->nid);
      if (in_array($node->type, $tax->taxed_product_types)) {
        $amount += $item->price * $item->qty * $tax->rate;
      }
    }
  }
  $taxed_line_items = $tax->taxed_line_items;
  if (is_array($order->line_items) && is_array($taxed_line_items)) {
    foreach ($order->line_items as $key => $line_item) {
      if ($line_item['type'] == 'tax' && $line_item['title'] == $tax->name) {
        // Don't tax yourself.
        continue;
      }
      if (in_array($line_item['type'], $taxed_line_items)) {
        $amount += $line_item['amount'] * $tax->rate;
      }
    }
  }
  if (isset($taxed_line_items['tax'])) {
    foreach ($_SESSION['taxes'] as $other_tax) {
      $amount += $other_tax['amount'] * $tax->rate;
    }
  }
  if ($amount) {
    $line_item = (object)array('id' => $tax->id, 'name' => $tax->name, 'amount' => $amount, 'weight' => $tax->weight);
    return $line_item;
  }
}

/**
 * Implementation of hook_ca_predicate().
 *
 * Create a predicate for each event corresponding to a tax rule.
 */
function uc_taxes_ca_predicate() {
  $predicates = array();

  $taxes = uc_taxes_get_rates();
  foreach ($taxes as $tax) {
    $predicates['uc_taxes_'. $tax->id] = array(
      '#title' => $tax->name,
      '#class' => 'uc_taxes',
      '#trigger' => 'calculate_tax_'. $tax->id,
      '#status' => 1,
      '#actions' => array(
        array(
          '#name' => 'uc_taxes_action_apply_tax',
          '#title' => t('Apply @tax', array('@tax' => $tax->name)),
          '#argument_map' => array(
            'order' => 'order',
            'tax' => 'tax',
          ),
        ),
      ),
    );
  }

  return $predicates;
}
