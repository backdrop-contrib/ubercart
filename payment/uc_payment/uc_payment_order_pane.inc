<?php
// $Id$

/**
 * @file
 * This file contains the callbacks for the payment order pane supplied with
 * Ubercart and their corresponding helper functions.
 *
 * Order panes are defined using hook_order_pane() and use a callback to handle
 * the different processes involved in order viewing/editing.  The payment order
 * pane is defined in uc_payment_order_pane() in uc_payment.module.
 */

/**
 * Handle the Payment order pane.
 */
function uc_order_pane_payment($op, $arg1) {
  switch ($op) {
    case 'view':
      if (variable_get('uc_payment_tracking', TRUE)) {
        $output = t('Balance:') .' '. uc_currency_format(uc_payment_balance($arg1));
        $output .= ' ('. l(t('View'), 'admin/store/orders/'. $arg1->order_id .'/payments') .')<br />';
      }
      $method_name = _payment_method_data($arg1->payment_method, 'review');
      if (empty($method_name)) {
        $method_name = _payment_method_data($arg1->payment_method, 'name');
      }
      $output .= t('Method:') .' '. $method_name;
      $func = _payment_method_data($arg1->payment_method, 'callback');
      if (function_exists($func)) {
        $method_output = $func('order-view', $arg1);
        if (!empty($method_output)) {
          $output .= '<br />'. $method_output;
        }
      }
      return $output;

    case 'customer':
      $method_name = _payment_method_data($arg1->payment_method, 'review');
      if (empty($method_name)) {
        $method_name = _payment_method_data($arg1->payment_method, 'name');
      }
      $output = t('Method:') .' '. $method_name;
      $func = _payment_method_data($arg1->payment_method, 'callback');
      if (function_exists($func)) {
        $method_output = $func('customer-view', $arg1);
        if (!empty($method_output)) {
          $output .= '<br />'. $method_output;
        }
      }
      return $output;

    case 'edit-form':
      drupal_add_js('misc/progress.js');
      drupal_add_js("var def_payment_msg = '';", 'inline');
      
      $form['payment'] = array(
        '#type' => 'fieldset',
        '#title' => t("Modify 'Payment Info'"),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE
      );
      $methods = _payment_method_list();
      foreach ($methods as $method) {
        if ($method['backend'] == TRUE) {
          $options[$method['id']] = $method['name'];
        }
      }
      $form['payment']['payment_method'] = array(
        '#type' => 'select',
        '#title' => t('Payment method'),
        '#default_value' => $arg1->payment_method,
        '#options' => (is_array($options)) ? $options : array(t('None available')),
        '#attributes' => array('onchange' => "get_payment_details('". base_path() ."?q=admin/store/orders/". $arg1->order_id ."/payment_details/' + this.value);"),
        '#disabled' => (is_array($options)) ? FALSE : TRUE,
      );
      return $form;

    case 'edit-theme':
      drupal_add_js(drupal_get_path('module', 'uc_payment') .'/uc_payment.js');
      $methods = _payment_method_list();
      $js = 'var methods = {';
      foreach ($methods as $method) {
        if ($method['backend'] == TRUE) {
          $js .= '"'. $method['name'] .'": "' . $method['id'] .'", ';
        }
      }
      $js = rtrim($js, ' ,') .'};';
      drupal_add_js($js, 'inline');
      drupal_add_js("\$(document).ready( function () { get_payment_details('". base_path() ."?q=admin/store/orders/' + \$('#edit-order-id').val() + '/payment_details/' + \$('#edit-payment-method').val()); } );", 'inline');
      $output = '<table class="order-edit-table">';
      foreach (element_children($arg1['payment']) as $field) {
        $title = $arg1['payment'][$field]['#title'];
        $arg1['payment'][$field]['#title'] = NULL;
        $output .= '<tr><td class="oet-label">'. $title .':</td><td>'
                 . drupal_render($arg1['payment'][$field]) .'</td></tr>';
      }
      $output .= '</table><div id="payment_details"></div>';
      return $output;

    case 'edit-process':
      $changes['payment_method'] = $arg1['payment_method'];
      $func = _payment_method_data($arg1['payment_method'], 'callback');
      if (function_exists($func) && ($return = $func('edit-process', $arg1)) != NULL && is_array($return)) {
         $changes = array_merge($changes, $return);
      }
      return $changes;
  }
}

