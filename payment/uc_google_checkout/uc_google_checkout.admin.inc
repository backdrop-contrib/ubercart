<?php

/**
 * @file
 * Google Checkout administration menu items.
 */

/**
 * Form builder for administrative settings form.
 *
 * @ingroup forms
 */
function uc_google_checkout_settings($form, &$form_state) {
  $form['authentication'] = array(
    '#type' => 'fieldset',
    '#title' => t('Authentication'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['authentication']['uc_google_checkout_merchant_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Merchant ID'),
    '#default_value' => variable_get('uc_google_checkout_merchant_id', ''),
  );
  $form['authentication']['uc_google_checkout_merchant_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Merchant key'),
    '#default_value' => variable_get('uc_google_checkout_merchant_key', ''),
    '#description' => t('Used to sign cart information. Keep it secret, keep it safe.'),
  );

  $form['authentication']['sandbox'] = array(
    '#type' => 'fieldset',
    '#title' => t('Test environment settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['authentication']['sandbox']['uc_google_checkout_mode'] = array(
    '#type' => 'radios',
    '#title' => t('Test mode'),
    '#default_value' => variable_get('uc_google_checkout_mode', 'checkout'),
    '#options' => array('sandbox' => t('On'), 'checkout' => t('Off')),
  );
  $form['authentication']['sandbox']['uc_google_checkout_test_merchant_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Test merchant ID'),
    '#description' => t('Only needed for test mode. Click <a href="@url" target="_blank">here</a> for more info.', array('@url' => 'http://code.google.com/apis/checkout/developer/index.html#integration_overview')),
    '#default_value' => variable_get('uc_google_checkout_test_merchant_id', ''),
    '#required' => FALSE,
  );
  $form['authentication']['sandbox']['uc_google_checkout_test_merchant_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Test merchant key'),
    '#default_value' => variable_get('uc_google_checkout_test_merchant_key', ''),
    '#required' => FALSE,
  );

  $form['messages'] = array(
    '#type' => 'fieldset',
    '#title' => t('Customer messages'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['messages']['uc_google_checkout_order_cancel_reason'] = array(
    '#type' => 'textarea',
    '#title' => t('Reason for canceling order'),
    '#description' => t('This message will be sent with the cancelation notice through Google Checkout. Any comment given when the order is canceled will be sent as a separate message.'),
    '#default_value' => variable_get('uc_google_checkout_order_cancel_reason', t('Order canceled. See order comments at [uc_order:url] for more information.')),
  );

  if (module_exists('token')) {
    $form['messages']['token_tree'] = array(
      '#markup' => theme('token_tree', array('token_types' => array('uc_order', 'site', 'store'))),
    );
  }

  $form['button'] = array(
    '#type' => 'fieldset',
    '#title' => t('Button settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['button']['uc_google_checkout_button_color'] = array(
    '#type' => 'radios',
    '#title' => t('Button background color'),
    '#default_value' => variable_get('uc_google_checkout_button_color', 'trans'),
    '#options' => array('trans' => t('Transparent'), 'white' => t('White')),
  );
  $form['button']['uc_google_checkout_button_size'] = array(
    '#type' => 'select',
    '#title' => t('Button size'),
    '#options' => array('large' => t('Large'), 'medium' => t('Medium'), 'small' => t('Small')),
    '#default_value' => variable_get('uc_google_checkout_button_size', 'large'),
  );

  return system_settings_form($form);
}

/**
 * Form builder for shipping settings form.
 *
 * @ingroup forms
 */
function uc_google_checkout_shipping_settings($form, &$form_state) {
  $form['help'] = array(
    '#value' => t("For merchant-calculated shipping, Google Checkout requires a fallback price in case it can't get a calculated shipping rate in time. When the cart request is sent to Google Checkout, Ubercart pre-calculates the shipping rates using this default address. Choose a location that will cause an average shipping cost to be returned."),
  );

  $form['address'] = array(
    '#type' => 'uc_address',
    '#default_value' => array(
      'uc_google_checkout_delivery_city' => variable_get('uc_google_checkout_delivery_city', ''),
      'uc_google_checkout_delivery_zone' => variable_get('uc_google_checkout_delivery_zone', ''),
      'uc_google_checkout_delivery_country' => isset($form_state['values']['uc_google_checkout_delivery_country']) ? $form_state['values']['uc_google_checkout_delivery_country'] : variable_get('uc_google_checkout_delivery_country', ''),
      'uc_google_checkout_delivery_postal_code' => variable_get('uc_google_checkout_delivery_postal_code', ''),
    ),
    '#required' => FALSE,
    '#key_prefix' => 'uc_google_checkout_delivery',
  );

  return system_settings_form($form);
}

/**
 * Form builder for tax settings form.
 *
 * @see uc_google_checkout_taxes_settings_submit()
 * @see theme_uc_google_checkout_taxes_settings()
 * @ingroup forms
 */
function uc_google_checkout_taxes_settings($form, &$form_state) {
  $form['#tree'] = TRUE;

  $result = db_query("SELECT zone, rate, tax_shipping FROM {uc_gc_taxes} ORDER BY zone");
  foreach ($result as $tax) {
    $form['taxes'][$tax->zone]['delete'] = array(
      '#type' => 'checkbox',
      '#default_value' => FALSE,
    );
    $form['taxes'][$tax->zone]['zone'] = array(
      '#type' => 'value',
      '#value' => $tax->zone,
    );
    $form['taxes'][$tax->zone]['rate'] = array(
      '#type' => 'textfield',
      '#default_value' => $tax->rate,
      '#size' => 8
    );
    $form['taxes'][$tax->zone]['tax_shipping'] = array(
      '#type' => 'checkbox',
      '#default_value' => $tax->tax_shipping,
    );
  }

  $form['taxes']['new']['delete'] = array(
    '#type' => 'checkbox',
    '#disabled' => TRUE,
  );
  $form['taxes']['new']['zone'] = uc_zone_select('', NULL, 840);
  $form['taxes']['new']['rate'] = array(
    '#type' => 'textfield',
    '#default_value' => '',
    '#size' => 8
  );
  $form['taxes']['new']['tax_shipping'] = array(
    '#type' => 'checkbox',
    '#default_value' => FALSE,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save taxes'),
  );

  return $form;
}

/**
 * Themes tax settings form.
 *
 * @see uc_google_checkout_taxes_settings()
 * @ingroup themeable
 */
function theme_uc_google_checkout_taxes_settings($variables) {
  $form = $variables['form'];

  $header = array(t('Delete'), uc_get_field_name('zone'), t('Rate'), t('Tax shipping?'));

  $rows = array();
  foreach (element_children($form['taxes']) as $zone) {
    $row = array();
    $row[] = drupal_render($form['taxes'][$zone]['delete']);
    if ($zone == 'new') {
      $row[] = drupal_render($form['taxes'][$zone]['zone']);
    }
    else {
      $row[] = $form['taxes'][$zone]['zone']['#value'];
    }
    $row[] = drupal_render($form['taxes'][$zone]['rate']);
    $row[] = drupal_render($form['taxes'][$zone]['tax_shipping']);
    $rows[] = $row;
  }

  $output = theme('table', array('header' => $header, 'rows' => $rows));
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Form submission handler for tax settings form.
 *
 * @see uc_google_checkout_taxes_settings()
 */
function uc_google_checkout_taxes_settings_submit($form, &$form_state) {
  foreach ($form_state['values']['taxes'] as $zone => $tax_values) {
    if ($zone == 'new' && $tax_values['zone'] && $tax_values['rate']) {
      db_merge('uc_gc_taxes')
        ->key(array('zone' => uc_get_zone_code($tax_values['zone'])))
        ->fields(array(
          'rate' => $tax_values['rate'],
          'tax_shipping' => $tax_values['tax_shipping'],
        ))
        ->execute();
    }
    elseif ($tax_values['delete']) {
      db_delete('uc_gc_taxes')
        ->condition('zone', $tax_values['zone'])
        ->execute();
    }
    else {
      db_update('uc_gc_taxes')
        ->fields(array(
          'rate' => $tax_values['rate'],
          'tax_shipping' => $tax_values['tax_shipping'],
        ))
        ->condition('zone', $tax_values['zone'])
        ->execute();
    }
  }
}

/**
 * Menu callback function that presents payment terminal form to administrator.
 */
function uc_google_checkout_terminal($order) {
  $order_id = $order->order_id;
  $gc_balance = $order->gc_total;
  $payments = uc_payment_load_payments($order_id);
  if (is_array($payments)) {
    foreach ($payments as $payment) {
      if ($payment->method == 'Google Checkout') {
        $gc_balance -= $payment->amount;
      }
    }
  }

  $build['details']['order_total'] = array(
    '#type' => 'item',
    '#title' => t('Order total:'),
    '#markup' => theme('uc_price', array('price' => $order->order_total)),
  );
  $build['details']['balance'] = array(
    '#type' => 'item',
    '#title' => t('Balance:'),
    '#markup' => theme('uc_price', array('price' => uc_payment_balance($order))),
  );
  $build['details']['gco_total'] = array(
    '#type' => 'item',
    '#title' => t('Google Checkout total:'),
    '#markup' => theme('uc_price', array('price' => $order->gc_total)),
  );
  $build['details']['gco_balance'] = array(
    '#type' => 'item',
    '#title' => t('Google Checkout balance:'),
    '#markup' => theme('uc_price', array('price' => $gc_balance)),
  );

  if (in_array($order->financial_state, array('REVIEWING', 'CHARGEABLE', 'CHARGED'))) {
    $build['form'] = drupal_get_form('uc_google_checkout_terminal_form', $order, $gc_balance);
  }

  return $build;
}

/**
 * Form builder for virtual terminal form.
 *
 * @see uc_google_checkout_terminal_form_validate()
 * @see uc_google_checkout_terminal_form_submit()
 * @ingroup forms
 */
function uc_google_checkout_terminal_form($form, &$form_state, $order, $amount = 0) {
  if ($order->financial_state == 'CHARGED') {
    $form['action'] = array(
      '#type' => 'select',
      '#title' => t('Action'),
      '#default_value' => 'charge',
      '#options' => array(
        'charge' => t('Charge'),
        'refund' => t('Refund'),
      ),
    );
    $form['reason'] = array(
      '#type' => 'textfield',
      '#title' => t('Reason for refund'),
      '#weight' => 1,
    );
    $form['comment'] = array(
      '#type' => 'textarea',
      '#title' => t('Refund Comment'),
      '#weight' => 2,
    );
  }
  else {
    $form['action'] = array(
      '#type' => 'value',
      '#value' => 'charge',
    );
  }

  $form['amount'] = array(
    '#type' => 'uc_price',
    '#title' => t('Amount'),
    '#default_value' => $amount,
  );
  $form['gc_balance'] = array(
    '#type' => 'value',
    '#value' => $amount,
  );
  $form['gc_total'] = array(
    '#type' => 'value',
    '#value' => $order->gc_total,
  );
  $form['order_id'] = array(
    '#type' => 'hidden',
    '#value' => $order->order_id,
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#weight' => 10,
  );

  return $form;
}

/**
 * Form validation for virtual terminal form.
 *
 * @see uc_google_checkout_terminal_form()
 * @see uc_google_checkout_terminal_form_submit()
 */
function uc_google_checkout_terminal_form_validate($form, &$form_state) {
  if ($form_state['values']['action'] == 'charge' && $form_state['values']['amount'] > $form_state['values']['gc_balance']) {
    form_set_error('amount', t('Google does not allow charges greater than the balance.'));
  }
  if ($form_state['values']['action'] == 'refund' && $form_state['values']['amount'] > $form_state['values']['gc_total'] - $form_state['values']['gc_balance']) {
    form_set_error('amount', t('Google does not allow refunds greater than the amount already charged.'));
  }
  if ($form_state['values']['action'] == 'refund' && empty($form_state['values']['reason'])) {
    form_set_error('reason', t('A reason for refunding the customer is required.'));
  }

  $order = uc_order_load($form_state['values']['order_id']);
  if ($order === FALSE) {
    form_set_error('', t('Invalid order ID.  Unable to process payment.'));
  }
}

/**
 * Form submission handler for virtual terminal form.
 *
 * @see uc_google_checkout_terminal_form()
 * @see uc_google_checkout_terminal_form_validate()
 */
function uc_google_checkout_terminal_form_submit($form, &$form_state) {
  if ($form_state['values']['action'] == 'charge') {
    $form_state['redirect'] = uc_google_checkout_charge($form_state['values']['order_id'], $form_state['values']['amount']);
  }
  elseif ($form_state['values']['action'] == 'refund') {
    $form_state['redirect'] = uc_google_checkout_refund($form_state['values']['order_id'], $form_state['values']['amount'], $form_state['values']['reason'], $form_state['values']['comment']);
  }
}
