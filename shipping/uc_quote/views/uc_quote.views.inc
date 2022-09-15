<?php
/**
 * @file
 * Provide views data for for the uc_quote module.
 */

/**
 * Implements hook_views_data().
 */
function uc_quote_views_data() {

  $data['uc_quote_product_locations']['table']['group'] = t('Product addresses');

  $data['uc_quote_product_locations']['table']['base'] = array(
    'field' => 'nid',
    'title' => t('Product addresses'),
    'help' => t('List of product addresses and can be related to nodes.'),
  );

  $data['uc_quote_product_locations']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  $data['uc_quote_product_locations']['nid'] = array(
    'title' => t('Product address node'),
    'help' => t('The product address reference to a node.'),
    'relationship' => array(
      'base' => 'node',
      'base field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Product node ID'),
      'title' => t('Product node ID'),
      'help' => t('Relates the product address for a product to its nid.'),
    ),
  );

  $data['uc_quote_product_locations']['first_name'] = array(
    'title' => t('First name'),
    'help' => t('The address first name'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['uc_quote_product_locations']['last_name'] = array(
    'title' => t('Last name'),
    'help' => t('The address last name'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['uc_quote_product_locations']['company'] = array(
    'title' => t('Company'),
    'help' => t('The address company'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['uc_quote_product_locations']['street1'] = array(
    'title' => t('Street line 1'),
    'help' => t('The address street line 1'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['uc_quote_product_locations']['street2'] = array(
    'title' => t('Street line 2'),
    'help' => t('The address street line 2'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['uc_quote_product_locations']['city'] = array(
    'title' => t('City'),
    'help' => t('The address city'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['uc_quote_product_locations']['zone'] = array(
    'title' => t('State/province'),
    'help' => t('The address state/province'),
    'field' => array(
      'handler' => 'views_handler_field_uc_zone',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_uc_zone',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_uc_zone',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_uc_zone',
    ),
  );

  $data['uc_quote_product_locations']['postal_code'] = array(
    'title' => t('Postal code'),
    'help' => t('The address postal code'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['uc_quote_product_locations']['country'] = array(
    'title' => t('Country'),
    'help' => t('The address country'),
    'field' => array(
      'handler' => 'views_handler_field_uc_country',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_uc_country',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_uc_country',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_uc_country',
    ),
  );

  $data['uc_quote_product_locations']['phone'] = array(
    'title' => t('Phone'),
    'help' => t('The address phone'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  return $data;
}
