<?php

/**
 * @file
 * Rules default configurations for uc_stock.module.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function uc_stock_default_rules_configuration() {
  $configs = array();

  $rule = rules_reaction_rule();
  $rule->label = t('Decrement stock upon order submission');
  $rule->active = TRUE;
  $rule->event('uc_checkout_complete');
  $rule->action('uc_stock_action_decrement_stock', array('order:select' => 'order'));
  $configs['uc_stock_decrement_on_order'] = $rule;

  return $configs;
}
