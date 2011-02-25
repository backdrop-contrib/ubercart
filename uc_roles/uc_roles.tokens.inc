<?php

/**
 * @file
 * Token hooks.
 */

/**
 * Implements hook_token_info().
 */
function uc_roles_token_info() {
  $type = array(
    'name' => t('Role promotions'),
    'description' => t('Tokens related to purchased, temporary roles.'),
    'needs-data' => 'uc_role',
  );

  $role['expiration'] = array(
    'name' => t('Expiration'),
    'description' => t('The date the role will expire.'),
    'type' => 'date',
  );
  $role['name'] = array(
    'name' => t('Role'),
    'description' => t('The associated role name'),
  );

  return array(
    'types' => array('uc_role' => $type),
    'tokens' => array('uc_role' => $role),
  );
}

/**
 * Implements hook_tokens().
 */
function uc_roles_tokens($type, $tokens, $data = array(), $options = array()) {
  $values = array();

  $sanitize = !empty($options['sanitize']);

  if ($type == 'uc_role' && !empty($data['uc_role'])) {
    $object = $data['uc_role'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'expiration':
          $values[$original] = format_date($object->expiration, 'medium');
          break;
        case 'name':
          $values[$original] = $sanitize ? check_plain(_uc_roles_get_name($object->rid)) : _uc_roles_get_name($object->rid);
          break;
      }
    }

    if ($expiration_tokens = token_find_with_prefix($tokens, 'expiration')) {
      $values += token_generate('date', $expiration_tokens, array('date' => $object->expiration), $options);
    }
  }

  return $values;
}
