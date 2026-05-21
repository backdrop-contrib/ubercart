<?php
/**
 * @file
 * Administrative forms for the Paypal Checkout module.
 */

/**
 * Form callback: allows the user to capture a prior authorization.
 */
function uc_paypal_checkout_capture_form($form, &$form_state, $order, $payment) {
  $form_state['order'] = $order;
  $form_state['payment'] = $payment;

  $total = $order->order_total;
  $payments = uc_payment_load_payments($order->order_id);
  $balance = $total;
  foreach ($payments as $p) {
    $balance -= $p->amount;
  }


  if ($balance > 0 && $balance < $payment->amount) {
    $default_amount = $balance;
  }
  else {
    $default_amount = $payment->amount;
  }

  // Convert the price amount to a user friendly decimal value.
  $default_amount = number_format($default_amount, 2, '.', '');

  $description = implode('<br />', array(
    t('Authorization: @amount', array('@amount' => uc_currency_format($payment->amount))),
    t('Order balance: @balance', array('@balance' => uc_currency_format($balance))),
  ));

  $form['amount'] = array(
    '#type' => 'number',
    '#step' => 0.01,
    '#min' => 0,
    '#title' => t('Capture amount'),
    '#description' => $description,
    '#default_value' => $default_amount,
    '#field_suffix' => check_plain($order->currency),
    '#size' => 16,
  );

  $form = confirm_form($form,
    t('What amount do you want to capture?'),
    'admin/store/orders/' . $order->order_id . '/payments',
    '',
    t('Capture'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Validate handler: ensure a valid amount is given.
 */
function uc_paypal_checkout_capture_form_validate($form, &$form_state) {
  $payment = $form_state['payment'];
  $amount = $form_state['values']['amount'];

  // Ensure the amount is within the allowed limit for PayPal authorizations.
  $authorization_amount = $payment->amount;
  $authorization_amount_upper = $payment->amount + 75;

  if ($amount > $authorization_amount * 1.15 || $amount > $authorization_amount_upper) {
    form_set_error('amount', t('You cannot capture an amount $75 or 115% greater than the authorization amount in PayPal Checkout.'));
  }

  // If the authorization has expired, display an error message and redirect.
  if (REQUEST_TIME - $payment->received > 86400 * 29) {
    backdrop_set_message(t('This authorization has passed its 29 day limit and cannot be captured.'), 'error');
    backdrop_goto('admin/store/orders/' . $form_state['order']->order_id . '/payments');
  }
}

/**
 * Submit handler: process a prior authorization capture via PayPal Checkout.
 */
function uc_paypal_checkout_capture_form_submit($form, &$form_state) {
  $settings = config_get('uc_paypal_checkout.settings');
  $payment = $form_state['payment'];
  $order = $form_state['order'];
  $order_total = $order->order_total;
  $amount = $form_state['values']['amount'];
  $api_client = new PayPalCheckoutClient($settings);

  $params = array(
    'final_capture' => $order_total == $amount,
    'amount' => array(
      'value' => number_format($amount, 2, '.', ''),
      'currency_code' => $order->currency,
    ),
  );

  try {
    // If the transaction was authorized more than 3 days ago, we need to
    // re-authorize the payment.
    $original_data = unserialize($payment->data);
    $remote_id = $original_data['uc_paypal_checkout']['remote_id'];
    if (time() >= ($payment->received + (86400 * 3))) {
      $api_client->reAuthorizePayment($remote_id, array('amount' => $params['amount']));
    }
    $response = $api_client->capturePayment($remote_id, $params);
    $data['uc_paypal_checkout']['remote_status'] = $response['status'];
    $data['uc_paypal_checkout']['remote_id'] = $response['id'];
    // Note the capture in the payment message.
    $comment = t('Captured: @date. Remote ID: @remote_id', array('@date' => format_date(REQUEST_TIME, 'short'), '@remote_id' => $response['id'] ));
    // Save a new payment receipt. Ubercart does not offer API to update payment
    // receipts.
    uc_payment_enter($order->order_id, 'paypal_checkout', $amount, 0, $data, $comment);
    // Remove old payment receipt.
    uc_payment_delete($payment->receipt_id);
    // If remote status is COMPLETED, then update the order.
    if ($response['status'] == 'COMPLETED') {
      $new_status = uc_order_state_default('payment_received');
      uc_order_update_status($order->order_id, $new_status);
    }
  }
  catch (PayPalCheckoutHttpException $exception) {
    watchdog_exception('uc_paypal_checkout', $exception);
    // Display an error message but leave the transaction pending.
    backdrop_set_message(t('Prior authorization capture failed, so the transaction will remain in a pending status.'), 'error');
  }

  // Redirect back to the current order payment page.
  $form_state['redirect'] = 'admin/store/orders/' . $form_state['order']->order_id . '/payments';
}

/**
 * Form callback: allows the user to void a transaction.
 */
function uc_paypal_checkout_void_form($form, &$form_state, $order, $payment) {
  $form_state['order'] = $order;
  $form_state['payment'] = $payment;
  $form['markup'] = array(
    '#markup' => t('Are you sure that you want to void this transaction?'),
  );

  $form = confirm_form($form,
    t('Are you sure that you want to void this transaction?'),
    'admin/store/orders/' . $order->order_id . '/payments',
    '',
    t('Void'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit handler: process the void request.
 */
function uc_paypal_checkout_void_form_submit($form, &$form_state) {
  $payment = $form_state['payment'];
  $order = $form_state['order'];
  $settings = config_get('uc_paypal_checkout.settings');
  $api_client = new PayPalCheckoutClient($settings);

  try {
    // This API call doesn't return content, so nothing to check, if no
    // exception is thrown, this means the void operation succeeded.
    $original_data = unserialize($payment->data);
    $api_client->voidPayment($original_data['uc_paypal_checkout']['remote_id']);
    backdrop_set_message(t('Transaction successfully voided.'));

    $data = $original_data;
    $data['uc_paypal_checkout']['remote_status'] = 'void';
    $comment = $payment->comment . '<br />' . t('Voided: @date', array('@date' => format_date(REQUEST_TIME, 'short')));
    uc_payment_enter($order->order_id, 'paypal_checkout', 0, 0, $data, $comment);
    uc_order_update_status($order->order_id, 'paypal_checkout_pending');
    uc_payment_delete($payment->receipt_id);
    rules_invoke_event('uc_paypal_checkout_event_voided', $order);

  }
  catch (PayPalCheckoutHttpException $exception) {
    backdrop_set_message(t('The void operation failed, so the payment will remain in a pending status.'), 'error');
  }
  $form_state['redirect'] = 'admin/store/orders/' . $form_state['order']->order_id . '/payments';
}

/**
 * Form callback: allows the user to issue a credit on a prior transaction.
 */
function uc_paypal_checkout_refund_form($form, &$form_state, $order, $payment) {
  $form_state['order'] = $order;
  $form_state['payment'] = $payment;

  $default_amount = number_format($payment->amount, 2, '.', '');

  $form['amount'] = array(
    '#type' => 'number',
    '#step' => 0.01,
    '#min' => 0,
    '#title' => t('Refund amount'),
    '#description' => t('Enter the amount to be refunded back to the PayPal account.'),
    '#default_value' => $default_amount,
    '#field_suffix' => check_plain($order->currency),
    '#size' => 16,
  );

  $form = confirm_form($form,
    t('What amount do you want to refund?'),
    'admin/store/orders/' . $order->order_id . '/payments',
    '',
    t('Refund'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Validate handler: check the credit amount before attempting a refund request.
 */
function uc_paypal_checkout_refund_form_validate($form, &$form_state) {
  $payment = $form_state['payment'];
  $amount = $form_state['values']['amount'];

  // Ensure the amount is less than or equal to the captured amount.
  if ($amount > $payment->amount) {
    form_set_error('amount', t('You cannot refund more than you captured.'));
  }

  // If the transaction is older than 60 days, display an error message and redirect.
  if (time() - $payment->received > 86400 * 60) {
    backdrop_set_message(t('This transaction has passed its 60 day limit for issuing refunds.'), 'error');
    backdrop_goto('admin/store/orders/' . $form_state['order']->order_id . '/payments');
  }
}

/**
 * Submit handler: process a refund request.
 */
function uc_paypal_checkout_refund_form_submit($form, &$form_state) {
  $payment = $form_state['payment'];
  $settings = config_get('uc_paypal_checkout.settings');
  $order = $form_state['order'];
  $api_client = new PayPalCheckoutClient($settings);
  $amount = $form_state['values']['amount'];

  $params = array(
    'amount' => array(
      'value' => $amount,
      'currency_code' => $order->currency,
    ),
  );
  try {
    $original_data = unserialize($payment->data);
    $response = $api_client->refundPayment($original_data['uc_paypal_checkout']['remote_id'], $params);
    // Not sure this can happen...
    if (strtolower($response['status']) != 'completed') {
      backdrop_set_message(t('Refund failed'));
    }
    else {
      backdrop_set_message(t('Refund for @amount issued successfully.', array('@amount' => $amount)));

      // Create a new payment record.
      $comment = t('Refunded to @remote_id.', array('@remote_id' => $original_data['uc_paypal_checkout']['remote_id']));
      $data = $original_data;
      $data['uc_paypal_checkout']['remote_id'] = $response['id'];
      $data['uc_paypal_checkout']['remote_status'] = 'refunded';
      uc_payment_enter($order->order_id, 'paypal_checkout', $amount * -1, 0, $data, $comment);
      rules_invoke_event('uc_paypal_checkout_event_refunded', $order);
    }
  }
  catch (PayPalCheckoutHttpException $exception) {
    watchdog_exception('uc_paypal_checkout', $exception);
    backdrop_set_message(t('Refund failed'), 'error');
  }

  $form_state['redirect'] = 'admin/store/orders/' . $order->order_id . '/payments';
}

/**
 * Determines access to the prior authorization capture form or void form for
 * Paypal Checkout transactions.
 *
 * @param UcOrder $order
 *   The order the transaction is on.
 * @param stdClass $payment
 *   The payment transaction object to be captured.
 *
 * @return
 *   TRUE or FALSE indicating access.
 */
function uc_paypal_checkout_capture_void_access($order, $payment) {
  // Return FALSE if the transaction isn't for Paypal Checkout or isn't
  // awaiting capture.
  $data = unserialize($payment->data);
  if ($payment->method != 'PayPal Checkout' || strtolower($data['uc_paypal_checkout']['remote_status']) != 'created') {
    return FALSE;
  }

  // Return FALSE if it is more than 29 days past the original authorization.
  if (REQUEST_TIME - $payment->received > 86400 * 29) {
    return FALSE;
  }

  // Allow access if the user can update payments on this transaction.
  // In Ubercart there is no separate permission for updating payments.
  return user_access('delete payments');
}

/**
 * Determines access to the refund form for Paypal Checkout transactions.
 *
 * @param $order
 *   The order the transaction is on.
 * @param $transaction
 *   The payment transaction object to be captured.
 *
 * @return
 *   TRUE or FALSE indicating access.
 */
function uc_paypal_checkout_refund_access($order, $payment) {
  $data = unserialize($payment->data);
  // Return FALSE if the transaction isn't Completed.
  if ($payment->method != 'PayPal Checkout' || strtolower($data['uc_paypal_checkout']['remote_status']) != 'completed') {
    return FALSE;
  }

  // Return FALSE if it is more than 60 days since the original transaction.
  if (REQUEST_TIME - $payment->received > 86400 * 60) {
    return FALSE;
  }

  // Allow access if the user can update payments on this transaction.
  // In Ubercart there is no separate permission for updating payments.
  return user_access('delete payments');
}
