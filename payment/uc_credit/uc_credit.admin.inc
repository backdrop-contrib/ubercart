<?php

/**
 * @file
 * Credit administration menu items.
 *
 */

/**
 * Displays the credit card terminal page.
 */
function uc_credit_terminal($order) {
  $build['return'] = array(
    '#markup' => l(t('Return to order view screen.'), 'admin/store/orders/' . $order->order_id),
  );

  $build['instructions'] = array('#markup' => '<p>' . t('Use this terminal to process credit card payments through your default gateway.') . '</p>');

  if (!variable_get('uc_credit_debug', FALSE) || variable_get('uc_credit_checkout_process', TRUE)) {
    $build['warning'] = array('#markup' => '<p>' . t('Be warned that credit card data will automatically be converted to the last 4 digits of the card once a transaction has occurred.  As such, subsequent charges after a partial payment will not have any stored credit card information to use.') . '</p>');
  }

  $build['details']['order_total'] = array('#markup' => '<div><strong>' . t('Order total: @total', array('@total' => uc_currency_format($order->order_total))) . '</strong></div>');
  $build['details']['balance'] = array('#markup' => '<div><strong>' . t('Balance: @balance', array('@balance' => uc_currency_format(uc_payment_balance($order)))) . '</strong></div>');

  $build['form'] = drupal_get_form('uc_credit_terminal_form', $order);

  return $build;
}

/**
 * Displays the credit card terminal form for administrators.
 */
function uc_credit_terminal_form($form, &$form_state, $order, $lock_amount = FALSE) {
  // Get the transaction types available to our default gateway.
  $types = uc_credit_gateway_txn_types(uc_credit_default_gateway());

  // Put the order ID in the form.
  $form['order_id'] = array(
    '#type' => 'hidden',
    '#value' => $order->order_id,
  );

  $balance = uc_payment_balance($order);

  // Let the administrator set the amount to charge.
  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Charge Amount'),
    '#default_value' => $balance > 0 ? uc_currency_format($balance, FALSE, FALSE, '.') : 0,
    '#size' => 10,
    '#disabled' => $lock_amount,
    '#field_prefix' => variable_get('uc_sign_after_amount', FALSE) ? '' : variable_get('uc_currency_sign', '$'),
    '#field_suffix' => variable_get('uc_sign_after_amount', FALSE) ? variable_get('uc_currency_sign', '$') : '',
  );

  // Build a credit card form.
  $form['specify_card'] = array(
    '#type' => 'fieldset',
    '#title' => t('Credit card details'),
    '#description' => t('Use the available buttons in this fieldset to process with the specified card details.'),
  );
  $form['specify_card']['cc_data'] = array(
    '#theme' => 'uc_payment_method_credit_form',
    '#tree' => TRUE,
  );
  $form['specify_card']['cc_data'] += uc_payment_method_credit_form(array(), $form_state, $order);
  unset($form['specify_card']['cc_data']['cc_policy']);

  $form['specify_card']['actions'] = array('#type' => 'actions');

  // If available, let the card be charged now.
  if (in_array(UC_CREDIT_AUTH_CAPTURE, $types)) {
    $form['specify_card']['actions']['charge_card'] = array(
      '#type' => 'submit',
      '#value' => t('Charge amount'),
    );
  }

  // If available, let the amount be authorized.
  if (in_array(UC_CREDIT_AUTH_ONLY, $types)) {
    $form['specify_card']['actions']['authorize_card'] = array(
      '#type' => 'submit',
      '#value' => t('Authorize amount only'),
    );
  }

  // If available, create a reference at the gateway.
  if (in_array(UC_CREDIT_REFERENCE_SET, $types)) {
    $form['specify_card']['actions']['reference_set'] = array(
      '#type' => 'submit',
      '#value' => t('Set a reference only'),
    );
  }

  // If available, create a reference at the gateway.
  if (in_array(UC_CREDIT_CREDIT, $types)) {
    $form['specify_card']['actions']['credit_card'] = array(
      '#type' => 'submit',
      '#value' => t('Credit amount to this card'),
    );
  }

  // Find any uncaptured authorizations.
  $options = array();

  if (isset($order->data['cc_txns']['authorizations'])) {
    foreach ($order->data['cc_txns']['authorizations'] as $auth_id => $data) {
      if (empty($data['captured'])) {
        $options[$auth_id] = t('@auth_id - @date - @amount authorized', array('@auth_id' => strtoupper($auth_id), '@date' => format_date($data['authorized'], 'short'), '@amount' => uc_currency_format($data['amount'])));
      }
    }
  }

  // If any authorizations existed...
  if (!empty($options)) {
    // Display a fieldset with the authorizations and available action buttons.
    $form['authorizations'] = array(
      '#type' => 'fieldset',
      '#title' => t('Prior authorizations'),
      '#description' => t('Use the available buttons in this fieldset to select and act on a prior authorization. The charge amount specified above will be captured against the authorization listed below.  Only one capture is possible per authorization, and a capture for more than the amount of the authorization may result in additional fees to you.'),
    );

    $form['authorizations']['select_auth'] = array(
      '#type' => 'radios',
      '#title' => t('Select authorization'),
      '#options' => $options,
    );

    $form['authorizations']['actions'] = array('#type' => 'actions');

    // If available, capture a prior authorization.
    if (in_array(UC_CREDIT_PRIOR_AUTH_CAPTURE, $types)) {
      $form['authorizations']['actions']['auth_capture'] = array(
        '#type' => 'submit',
        '#value' => t('Capture amount to this authorization'),
      );
    }

    // If available, void a prior authorization.
    if (in_array(UC_CREDIT_VOID, $types)) {
      $form['authorizations']['actions']['auth_void'] = array(
        '#type' => 'submit',
        '#value' => t('Void authorization'),
      );
    }

    // Collapse this fieldset if no actions are available.
    if (!isset($form['authorizations']['actions']['auth_capture']) && !isset($form['authorizations']['actions']['auth_void'])) {
      $form['authorizations']['#collapsible'] = TRUE;
      $form['authorizations']['#collapsed'] = TRUE;
    }
  }

  // Find any uncaptured authorizations.
  $options = array();

  // Log a reference to the order for testing.
  // $order->data = uc_credit_log_reference($order->order_id, substr(md5(REQUEST_TIME), 0, 16), '4111111111111111');

  if (isset($order->data['cc_txns']['references'])) {
    foreach ($order->data['cc_txns']['references'] as $ref_id => $data) {
      $options[$ref_id] = t('@ref_id - @date - (Last 4) @card', array('@ref_id' => strtoupper($ref_id), '@date' => format_date($data['created'], 'short'), '@card' => $data['card']));
    }
  }

  // If any references existed...
  if (!empty($options)) {
    // Display a fieldset with the authorizations and available action buttons.
    $form['references'] = array(
      '#type' => 'fieldset',
      '#title' => t('Customer references'),
      '#description' => t('Use the available buttons in this fieldset to select and act on a customer reference.'),
    );

    $form['references']['select_ref'] = array(
      '#type' => 'radios',
      '#title' => t('Select references'),
      '#options' => $options,
    );

    $form['references']['actions'] = array('#type' => 'actions');

    // If available, capture a prior references.
    if (in_array(UC_CREDIT_REFERENCE_TXN, $types)) {
      $form['references']['actions']['ref_capture'] = array(
        '#type' => 'submit',
        '#value' => t('Charge amount to this reference'),
      );
    }

    // If available, remove a previously stored reference.
    if (in_array(UC_CREDIT_REFERENCE_REMOVE, $types)) {
      $form['references']['actions']['ref_remove'] = array(
        '#type' => 'submit',
        '#value' => t('Remove reference'),
      );
    }

    // If available, remove a previously stored reference.
    if (in_array(UC_CREDIT_REFERENCE_CREDIT, $types)) {
      $form['references']['actions']['ref_credit'] = array(
        '#type' => 'submit',
        '#value' => t('Credit amount to this reference'),
      );
    }

    // Collapse this fieldset if no actions are available.
    if (!isset($form['references']['actions']['ref_capture']) && !isset($form['references']['actions']['ref_remove']) && !isset($form['references']['actions']['ref_credit'])) {
      $form['references']['#collapsible'] = TRUE;
      $form['references']['#collapsed'] = TRUE;
    }
  }

  return $form;
}

/**
 * Validation handler for credit terminal form.
 */
function uc_credit_terminal_form_validate($form, &$form_state) {
  switch ($form_state['values']['op']) {
    case t('Charge amount'):
    case t('Authorize amount only'):
    case t('Capture amount to this authorization'):
    case t('Charge amount to this reference'):
      if (!is_numeric($form_state['values']['amount']) || $form_state['values']['amount'] <= 0) {
        form_set_error('amount', t('You must enter a positive number for the amount.'));
      }
  }

  if (uc_order_load($form_state['values']['order_id']) === FALSE) {
    form_set_error('', t('Invalid order ID.  Unable to process payment.'));
  }
}

/**
 * Submit handler for credit terminal form.
 */
function uc_credit_terminal_form_submit($form, &$form_state) {
  // Load the order.
  $order = uc_order_load($form_state['values']['order_id']);

  // Get the data from the form and replace masked data from the order.
  $cc_data = $form_state['values']['cc_data'];

  if (strpos($cc_data['cc_number'], t('(Last 4) ')) === 0) {
    $cc_data['cc_number'] = $order->payment_details['cc_number'];
  }

  if (isset($cc_data['cc_cvv']) && isset($order->payment_details['cc_cvv'])) {
    if ($cc_data['cc_cvv'] == str_repeat('-', strlen($cc_data['cc_cvv']))) {
      $cc_data['cc_cvv'] = $order->payment_details['cc_cvv'];
    }
  }

  // Cache the values for use during processing.
  uc_credit_cache('save', $cc_data, FALSE);

  // Build the data array passed on to the payment gateway.
  $data = array();

  switch ($form_state['values']['op']) {
    case t('Charge amount'):
      $data['txn_type'] = UC_CREDIT_AUTH_CAPTURE;
      break;

    case t('Authorize amount only'):
      $data['txn_type'] = UC_CREDIT_AUTH_ONLY;
      break;

    case t('Set a reference only'):
      $data['txn_type'] = UC_CREDIT_REFERENCE_SET;
      break;

    case t('Credit amount to this card'):
      $data['txn_type'] = UC_CREDIT_CREDIT;
      break;

    case t('Capture amount to this authorization'):
      $data['txn_type'] = UC_CREDIT_PRIOR_AUTH_CAPTURE;
      $data['auth_id'] = $form_state['values']['select_auth'];
      break;

    case t('Void authorization'):
      $data['txn_type'] = UC_CREDIT_VOID;
      $data['auth_id'] = $form_state['values']['select_auth'];
      break;

    case t('Charge amount to this reference'):
      $data['txn_type'] = UC_CREDIT_REFERENCE_TXN;
      $data['ref_id'] = $form_state['values']['select_ref'];
      break;

    case t('Remove reference'):
      $data['txn_type'] = UC_CREDIT_REFERENCE_REMOVE;
      $data['ref_id'] = $form_state['values']['select_ref'];
      break;

    case t('Credit amount to this reference'):
      $data['txn_type'] = UC_CREDIT_REFERENCE_CREDIT;
      $data['ref_id'] = $form_state['values']['select_ref'];
  }

  $result = uc_payment_process_payment('credit', $form_state['values']['order_id'], $form_state['values']['amount'], $data, TRUE, NULL, FALSE);

  if ($result) {
    $crypt = new uc_encryption_class;

    // Load up the existing data array.
    $data = db_query("SELECT data FROM {uc_orders} WHERE order_id = :id", array(':id' => $form_state['values']['order_id']))->fetchField();
    $data = unserialize($data);

    $cache = uc_credit_cache('load');

    if (variable_get('uc_credit_debug', FALSE) && !variable_get('uc_credit_checkout_process', TRUE)) {
      $cc_data = $cache;
    }
    else {
      $cc_data = array(
        'cc_number' => substr($cache['cc_number'], -4),
      );
    }

    // Stuff the serialized and encrypted CC details into the array.
    $data['cc_data'] = $crypt->encrypt(uc_credit_encryption_key(), serialize($cc_data));
    uc_store_encryption_errors($crypt, 'uc_credit');

    // Save it again.
    db_update('uc_orders')
      ->fields(array('data' => serialize($data)))
      ->condition('order_id', $form_state['values']['order_id'])
      ->execute();

    drupal_set_message(t('The credit card was processed successfully. See the admin comments for more details.'));
  }
  else {
    if (variable_get('uc_credit_debug', FALSE)) {
      _uc_credit_save_cc_data_to_order(uc_credit_cache('load'), $form_state['values']['order_id']);
    }

    drupal_set_message(t('There was an error processing the credit card.  See the admin comments for details.'), 'error');
  }

  $form_state['redirect'] = 'admin/store/orders/' . $form_state['values']['order_id'];
}
