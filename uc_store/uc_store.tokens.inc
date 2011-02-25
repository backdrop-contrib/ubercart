<?php

/**
 * @file
 * Token hooks.
 */

/**
 * Implements hook_token_info().
 */
function uc_store_token_info() {
  $type = array(
    'name' => t('Store information'),
    'description' => t('Tokens for store-specific, but globally available, information.'),
  );

  $site['login-link'] = array(
    'name' => t('Login URL'),
    'description' => t('A link to the site login page.'),
  );
  $site['logo'] = array(
    'name' => t('Logo'),
    'description' => t('The image showing the site logo.'),
  );

  $store['name'] = array(
    'name' => t('Store name'),
    'description' => t('The Ubercart store name.'),
    );
  $store['url'] = array(
    'name' => t('URL'),
    'description' => t('The Ubercart store URL.'),
  );
  $store['link'] = array(
    'name' => t('Store link'),
    'description' => t('A link to the Ubercart store using the store name.'),
  );
  $store['owner'] = array(
    'name' => t('Owner'),
    'description' => t('The Ubercart store owner.'),
  );
  $store['email'] = array(
    'name' => t('Email'),
    'description' => t('The Ubercart store e-mail address.'),
  );
  $store['phone'] = array(
    'name' => t('Phone number'),
    'description' => t('The Ubercart store phone number.'),
  );
  $store['fax'] = array(
    'name' => t('Fax Number'),
    'description' => t('The Ubercart store fax number.'),
  );
  $store['address'] = array(
    'name' => t('Address'),
    'description' => t('The Ubercart store mailing address.'),
  );
  $store['help-url'] = array(
    'name' => t('Help page URL'),
    'description' => t('The URL to the store help page.'),
  );

  return array(
    'types' => array('store' => $type),
    'tokens' => array(
      'site' => $site,
      'store' => $store,
    ),
  );
}

/**
 * Implements hook_tokens().
 */
function uc_store_tokens($type, $tokens, $data = array(), $options = array()) {
  global $base_url;

  $values = array();

  if ($type == 'site') {
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'login-link':
          $login_link = url('user', array('absolute' => TRUE));
          $values[$original] = l($login_link, $login_link);
          break;

        case 'logo':
          // Use a logo; but only if we have one to use.
          if ($uri = theme_get_setting('logo')) {
            $values[$original] = '<img src="' . $uri . '" />';
          }
          break;
      }
    }
  }

  if ($type == 'store') {
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'name':
          $values[$original] = variable_get('uc_store_name', t('Our store'));
          break;

        case 'url':
          $values[$original] = $base_url;
          break;

        case 'link':
          $values[$original] = l(variable_get('uc_store_name', t('Our store')), $base_url);
          break;

        case 'owner':
          $values[$original] = variable_get('uc_store_owner', '');
          break;

        case 'email':
          $values[$original] = variable_get('uc_store_email', '');
          break;

        case 'phone':
          $values[$original] = variable_get('uc_store_phone', '');
          break;

        case 'fax':
          $values[$original] = variable_get('uc_store_fax', '');
          break;

        case 'address':
          $values[$original] = uc_store_address();
          break;

        case 'help-url':
          $values[$original] = url(variable_get('uc_store_help_page', ''), array('absolute' => TRUE));
          break;
      }
    }
  }

  return $values;
}
