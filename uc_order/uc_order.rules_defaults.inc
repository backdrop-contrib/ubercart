<?php
/**
 * @file
 * Default rules configurations.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function uc_order_default_rules_configuration() {
  // During D7->B upgrade, there may be no filter formats when this gets called,
  // so check for the existence of filter formats.
  if (!_uc_order_filter_formats_exist()) {
    return array();
  }

  $rule = rules_reaction_rule();
  $rule->label = t('E-mail an order update notification');
  $rule->active = TRUE;
  $rule->event('uc_order_status_email_update')
    ->condition(rules_condition('data_is', array('data:select' => 'order:order-status', 'value' => 'in_checkout'))->negate())
    ->action('uc_order_email', array(
      'order:select' => 'order',
      'from' => uc_store_email_from(),
      'addresses' => '[order:email]',
      'subject' => t('Order #[order:order-number] Update'),
      'message' => uc_get_message('order_update_email'),
      'format' => filter_default_format(),
    ));

  $configs['uc_order_update_email_customer'] = $rule;

  return $configs;
}

/**
 * Utility routine to determine whether filter formats exist (which is not the
 * case during D7->B upgrades).
 */
function _uc_order_filter_formats_exist() {
  // Check first in cache.
  global $language;
  $cache = cache()->get("filter_formats:{$language->langcode}");
  if (!empty($cache)) {
    return TRUE;
  }
  // Then check config.
  $config_names = config_get_names_with_prefix('filter.format.');
  if (!empty($config_names)) {
    return TRUE;
  }
  return FALSE;
}
