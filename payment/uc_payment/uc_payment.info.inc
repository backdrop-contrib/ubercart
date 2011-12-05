<?php

/**
 * @file
 * Entity Metadata hooks
 */

/**
 * Implements hook_entity_property_info_alter().
 */
function uc_payment_entity_property_info_alter(&$info) {
  $info['uc_order']['properties']['payment_method']['options list'] = 'uc_payment_method_options_list';
}
