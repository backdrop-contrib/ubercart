<?php

/**
 * @file
 * Payment administration menu items.
 *
 */


/**
 * Displays an overview of the payment settings.
 */
function uc_payment_settings_overview() {
  // Theme all the pages beneath this path into summary overviews.
  return array(
    '#theme' => 'uc_summary_overview',
    '#summaries' => uc_summarize_child_form_pages('admin/store/settings/payment/edit'),
  );
}

function uc_payment_methods_form($form, &$form_state) {
  $methods = _uc_payment_method_list();

  $form['methods_info'] = array(
    '#markup' => '<div><strong>' . t('Payment methods') . '</strong><br />'
              . t('The settings forms below are for the payment methods defined by enabled modules. Click a name to expand its options and adjust the settings accordingly. Methods are listed in order of appearance on the checkout screen, determined by the list position setting (current value shown in parentheses).') . '</div><br />',
  );

  $form['pmtable'] = array(
    '#theme' => 'uc_payment_method_table',
    '#summary callback' => 'uc_summarize_form',
  );

  if (is_array($methods) && count($methods) > 0) {
    foreach ($methods as $id => $method) {

      $form['pmtable'][$id]['#summary callback'] = 'uc_summarize_form';

      $form['pmtable'][$id]['uc_payment_method_' . $id . '_checkout'] = array(
        '#type' => 'checkbox',
        '#summary callback' => 'uc_summarize_checkbox',
        '#summary arguments' => array(
          t('@payment is enabled for checkout.', array('@payment' => $method['name'])),
          t('@payment is disabled for checkout.', array('@payment' => $method['name'])),
        ),
        '#default_value' => variable_get('uc_payment_method_' . $id . '_checkout', $method['checkout']),
      );
      $form['pmtable'][$id]['name'] = array(
        '#markup' => $method['name'],
      );
      $form['pmtable'][$id]['uc_payment_method_' . $id . '_weight'] = array(
        '#type' => 'weight',
        '#default_value' => variable_get('uc_payment_method_' . $id . '_weight', $method['weight']),
      );

      if (isset($method['no_gateway']) && $method['no_gateway'] === TRUE) {
        $form['pmtable'][$id]['uc_payment_' . $id . '_gateway'] = array(
          '#markup' => '-',
        );
      }
      else {
        $gateways = _uc_payment_gateway_list($id, TRUE);
        $options = array();
        foreach ($gateways as $gateway_id => $gateway) {
          $options[$gateway_id] = $gateway['title'];
        }
        if (empty($options)) {
          $options = array('none' => t('None available.'));
        }
        $form['pmtable'][$id]['uc_payment_' . $id . '_gateway'] = array(
          '#type' => 'select',
          '#options' => $options,
          '#summary callback' => 'uc_summarize_null',
          '#default_value' => variable_get('uc_payment_' . $id . '_gateway', 'none'),
        );
      }

      $null = NULL;
      $method_settings = $method['callback']('settings', $null, array(), $form_state);
      if (is_array($method_settings)) {
        $form['method_' . $id] = array(
          '#type' => 'fieldset',
          '#summary callback' => 'uc_summarize_null',
          '#title' => t('!method settings', array('!method' => $method['name'], '!weight' => $method['weight'])),
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
        );
        $form['method_' . $id] = array_merge($form['method_' . $id], $method_settings);
      }
    }
  }

  return system_settings_form($form);
}

/**
 * @ingroup themeable
 */
function theme_uc_payment_method_table($variables) {
  $form = $variables['form'];

  $header = array(t('Enabled'), t('Payment method'), t('List position'), t('Default gateway'));

  foreach (element_children($form) as $method) {
    $rows[] = array(
      array('data' => drupal_render($form[$method]['uc_payment_method_' . $method . '_checkout']), 'align' => 'center'),
      drupal_render($form[$method]['name']),
      drupal_render($form[$method]['uc_payment_method_' . $method . '_weight']),
      drupal_render($form[$method]['uc_payment_' . $method . '_gateway']),
    );
  }

  if (empty($rows)) {
    $rows[] = array(
      array('data' => t('No payment methods founds.'), 'colspan' => 5),
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

function uc_payment_gateways_form($form, &$form_state) {
  $gateways = _uc_payment_gateway_list();
  $methods = _uc_payment_method_list();

  $form['gateways'] = array(
    '#summary callback' => '_uc_payment_gateways_summarize',
    '#summary arguments' => array($gateways),
  );

  if (is_array($gateways) && count($gateways) > 0) {
    $form['gateways_info'] = array(
      '#markup' => '<div><strong>' . t('Payment gateways') . '</strong><br />'
                . t('Payment gateways are web services that allow you to process various types of payments remotely.  The settings forms below are for the payment gateways you have installed.  Click a name to expand its options and adjust the settings accordingly.') . '</div>',
      '#weight' => -10,
    );

    foreach ($gateways as $id => $gateway) {
      $form['gateways'][$id] = array(
        '#type' => 'fieldset',
        '#title' => t('@gateway_name settings', array('@gateway_name' => $gateway['title'])),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $supported_methods = array();
      foreach ($methods as $method_id => $method) {
        if (isset($gateway[$method_id]) && function_exists($gateway[$method_id])) {
          $supported_methods[] = $method['name'];
        }
      }

      $form['gateways'][$id]['supported_methods'] = array(
        '#markup' => '<div>' . t('This gateway supports the following payment methods:')
                   . '<br />' . implode(',', $supported_methods) . '</div>',
        '#weight' => -10,
      );
      $form['gateways'][$id]['uc_pg_' . $id . '_enabled'] = array(
        '#type' => 'checkbox',
        '#title' => t('Enable this payment gateway for use.'),
        '#default_value' => variable_get('uc_pg_' . $id . '_enabled', TRUE),
      );

      // Find any additional settings defined in the payment gateway callback.
      if (isset($gateway['settings']) && function_exists($gateway['settings'])) {
        $gateway_settings = $gateway['settings'](array(), $form_state);
        if (is_array($gateway_settings)) {
          $form['gateways'][$id] = array_merge($form['gateways'][$id], $gateway_settings);
        }
      }
    }
  }
  else {
    $form['gateways_info'] = array(
      '#markup' => '<div><strong>' . t('Payment gateways') . '</strong><br />'
                . t('Payment gateways are web services that allow you to process various types of payments remotely. No payment gateways are currently enabled.') . '</div>',
      '#weight' => -10,
    );
  }

  return system_settings_form($form);
}

/**
 * Returns an array of enabled payment gateways for the form summary.
 */
function _uc_payment_gateways_summarize($form, $gateways) {
  $items = array();

  foreach ($gateways as $gateway) {
    $items[] = t('@title is @enabled.', array('@title' => $gateway['title'], '@enabled' => $gateway['enabled'] ? t('enabled') : t('disabled')));
  }

  if (empty($items)) {
    $items[] = t('No payment gateway modules are installed.');
  }

  return $items;
}

/**
 * Selects a payment gateway to process a payment when multiple gateways
 * exist for a given payment method.
 */
function uc_payment_gateway_select() {
  $gateways = _uc_payment_gateway_list($_SESSION['uc_payment_method'], TRUE);
  foreach ($gateways as $id => $gateway) {
    $options[$id] = $gateway['title'];
  }

  $build['instructions'] = array('#markup' => t('Please choose a payment gateway to use for that payment.'));

  $build['form'] = drupal_get_form('uc_payment_gateway_select_form', $options,
               $_SESSION['uc_payment_method'], $_SESSION['uc_payment_order_id'],
               $_SESSION['uc_payment_amount'], $_SESSION['uc_payment_data']);

  return $build;
}

function uc_payment_gateway_select_form($form, &$form_state, $options, $method, $order_id, $amount, $data) {
  $form['method'] = array(
    '#type' => 'hidden',
    '#value' => $method,
  );
  $form['order_id'] = array(
    '#type' => 'hidden',
    '#value' => $order_id,
  );
  $form['amount'] = array(
    '#type' => 'hidden',
    '#value' => $amount,
  );
  $form['p_data'] = array(
    '#type' => 'hidden',
    '#value' => $data,
  );
  $form['p_selected'] = array(
    '#type' => 'select',
    '#title' => t('Use gateway'),
    '#options' => $options,
    '#default_value' => variable_get('uc_payment_' . $method . '_gateway', ''),
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Process'),
  );

  return $form;
}

function uc_payment_gateway_select_form_submit($form, &$form_state) {
  unset($_SESSION['uc_payment_method']);
  unset($_SESSION['uc_payment_order_id']);
  unset($_SESSION['uc_payment_amount']);
  unset($_SESSION['uc_payment_data']);

  uc_payment_process_payment($form_state['values']['method'], $form_state['values']['order_id'],
    $form_state['values']['amount'], unserialize($form_state['values']['p_data']), FALSE,
    $form_state['values']['p_selected']);

  $form_state['redirect'] = 'admin/store/orders/' . $form_state['values']['order_id'];
}

/**
 * Displays a list of payments attached to an order.
 *
 * @see uc_payment_by_order_form_validate()
 * @see uc_payment_by_order_form_submit()
 * @see theme_uc_payment_by_order_form()
 * @ingroup forms
 */
function uc_payment_by_order_form($form, &$form_state, $order) {
  $total = $order->order_total;
  $payments = uc_payment_load_payments($order->order_id);

  $form['order_total'] = array('#theme' => 'uc_price', '#price' => $total);
  $form['payments'] = tapir_get_table('uc_payments_table');

  if ($payments !== FALSE) {
    foreach ($payments as $payment) {
      $form['payments'][$payment->receipt_id]['#attributes'] = array('valign' => 'top');
      $form['payments'][$payment->receipt_id]['received'] = array(
        '#markup' => format_date($payment->received, 'custom', variable_get('date_format_uc_store', 'm/d/Y') . '<b\r />H:i:s'),
      );
      $form['payments'][$payment->receipt_id]['user'] = array(
        '#markup' => uc_get_initials($payment->uid),
      );
      $form['payments'][$payment->receipt_id]['method'] = array(
        '#markup' => ($payment->method == '') ? t('Unknown') : $payment->method,
      );
      $form['payments'][$payment->receipt_id]['amount'] = array(
        '#theme' => 'uc_price',
        '#price' => $payment->amount,
      );
      $total -= $payment->amount;
      $form['payments'][$payment->receipt_id]['balance'] = array(
        '#theme' => 'uc_price',
        '#price' => $total,
      );
      $form['payments'][$payment->receipt_id]['comment'] = array(
        '#markup' => ($payment->comment == '') ? '-' : filter_xss_admin($payment->comment),
      );
      if (user_access('delete payments')) {
        $action_value = l(t('Delete'), 'admin/store/orders/' . $order->order_id . '/payments/'
                          . $payment->receipt_id . '/delete');
      }
      else {
        $action_value = '-';
      }
      $form['payments'][$payment->receipt_id]['action'] = array(
        '#markup' => $action_value,
      );
    }
  }

  $form['balance'] = array('#theme' => 'uc_price', '#price' => $total);
  $form['order_id'] = array(
    '#type' => 'hidden',
    '#value' => $order->order_id,
  );

  if (user_access('manual payments')) {
    $form['payments']['new']['#attributes'] = array('valign' => 'top');
    $form['payments']['new']['received'] = array(
      '#markup' => '-',
    );
    $form['payments']['new']['user'] = array(
      '#markup' => '-',
    );
    $methods = _uc_payment_method_list();
    foreach ($methods as $id => $method) {
      $options[$id] = $method['name'];
    }
    $form['payments']['new']['method'] = array(
      '#type' => 'select',
      '#options' => $options,
    );
    $form['payments']['new']['amount'] = array(
      '#type' => 'textfield',
      '#size' => 6,
    );
    $form['payments']['new']['balance'] = array(
      '#markup' => '-',
    );
    $form['payments']['new']['comment'] = array(
      '#type' => 'textfield',
      '#size' => 32,
      '#maxlength' => 256,
    );
    $form['payments']['new']['actions'] = array('#type' => 'actions');
    $form['payments']['new']['actions']['action'] = array(
      '#type' => 'submit',
      '#value' => t('Enter'),
    );
  }

  return $form;
}

/**
 * Themes uc_payment_by_order_form().
 *
 * @see uc_payment_by_order_form()
 * @ingroup themeable
 */
function theme_uc_payment_by_order_form($variables) {
  $form = $variables['form'];

  $output = '<p><strong>' . t('Order total:') . '</strong> ' . drupal_render($form['order_total'])
           . '<br /><strong>' . t('Current balance:') . '</strong> '
          . drupal_render($form['balance']) . '</p>';
  $output .= '<p>' . drupal_render($form['payments']) . '</p>'
           . '<p>' . drupal_render($form['form_id'])
           . drupal_render($form['form_token']) . '</p>';
  return $output;
}

/**
 * Form validation for uc_payment_by_order_form().
 *
 * @see uc_payment_by_order_form()
 * @see uc_payment_by_order_form_submit()
 */
function uc_payment_by_order_form_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['payments']['new']['amount'])) {
    form_set_error('payments][new][amount', t('You must enter a number for the amount.'));
  }
  return TRUE;
}

/**
 * Form submission handler for uc_payment_by_order_form().
 *
 * @see uc_payment_by_order_form()
 * @see uc_payment_by_order_form_validate()
 */
function uc_payment_by_order_form_submit($form, &$form_state) {
  global $user;

  $payment = $form_state['values']['payments']['new'];
  uc_payment_enter($form_state['values']['order_id'], $payment['method'], $payment['amount'],
                   $user->uid, '', $payment['comment']);

  drupal_set_message(t('Payment entered.'));
}

/**
 * Confirmation form to delete a payment from an order.
 *
 * @see uc_payment_delete_confirm_form_submit()
 * @ingroup forms
 */
function uc_payment_delete_confirm_form($form, &$form_state, $order, $payment) {
  // Make sure the payment is for the specified order.
  if ($payment->order_id != $order->order_id) {
    drupal_set_message(t('An error loading the payment information occurred.'));
    drupal_goto('admin/store/orders/' . $order->order_id . '/payments');
  }

  $desc = '<strong>' . t('Payment information:') . '</strong> '
        . t('@method payment of @amount received on @date.', array('@method' => $payment->method, '@amount' => uc_currency_format($payment->amount), '@date' => format_date($payment->received, 'short')));

  $form['order_id'] = array(
    '#type' => 'value',
    '#value' => $order->order_id
  );
  $form['receipt_id'] = array(
    '#type' => 'value',
    '#value' => $payment->receipt_id,
  );

  return confirm_form($form, t('Are you sure you want to delete this payment?'), 'admin/store/orders/' . $order->order_id . '/payments', $desc, t('Delete'));
}

/**
 * Form submission handler for uc_payment_delete_confirm_form().
 *
 * @see uc_payment_delete_confirm_form()
 */
function uc_payment_delete_confirm_form_submit($form, &$form_state) {
  uc_payment_delete($form_state['values']['receipt_id']);

  drupal_set_message(t('Payment deleted.'));

  $form_state['redirect'] = 'admin/store/orders/' . $form_state['values']['order_id'] . '/payments';
}
