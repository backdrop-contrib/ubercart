<?php

/**
 * @file
 * Credit administration menu items.
 *
 */

/**
 * Credit card settings form.
 */
function uc_credit_settings_form($form, &$form_state) {
  if (!user_access('administer credit cards')) {
    $form['notice'] = array(
      '#markup' => '<div>' . t('You must have access to <b>administer credit cards</b> to adjust these settings.') . '</div>',
    );
    return $form;
  }

  $gateways = _uc_payment_gateway_list('credit');
  if (!count($gateways)) {
    $form['notice'] = array(
      '#markup' => '<div>' . t('Please enable a credit card gateway module for your chosen payment provider.') . '</div>',
    );
    return $form;
  }

  $form['uc_credit'] = array('#type' => 'vertical_tabs');

  $form['cc_basic'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic settings'),
    '#group' => 'uc_credit',
  );
  $options = array();
  foreach ($gateways as $id => $gateway) {
    $options[$id] = $gateway['title'];
  }
  $form['cc_basic']['uc_payment_credit_gateway'] = array(
    '#type' => 'radios',
    '#title' => t('Default gateway'),
    '#options' => $options,
    '#default_value' => variable_get('uc_payment_credit_gateway', 'none'),
  );
  $form['cc_basic']['uc_credit_validate_numbers'] = array(
    '#type' => 'checkbox',
    '#title' => t('Validate credit card numbers at checkout.'),
    '#description' => t('Invalid card numbers will show an error message to the user so they can correct it.<br />This feature is recommended unless you are in debug mode.'),
    '#default_value' => variable_get('uc_credit_validate_numbers', TRUE),
  );
  $form['cc_basic']['uc_credit_checkout_process'] = array(
    '#type' => 'checkbox',
    '#title' => t('Attempt to process credit card payments at checkout.'),
    '#description' => t('Failed attempts will prevent checkout completion and display the error message from above.<br />This box must be checked to process customer credit cards if you are not in debug mode.'),
    '#default_value' => variable_get('uc_credit_checkout_process', TRUE),
  );

  // Form elements that deal specifically with card number security.
  $form['cc_security'] = array(
    '#type' => 'fieldset',
    '#title' => t('Security settings'),
    '#description' => t('You are responsible for the security of your website, including the protection of credit card numbers.  Please be aware that choosing some settings in this section may decrease the security of credit card data on your website and increase your liability for damages in the case of fraud.'),
    '#group' => 'uc_credit',
  );
  $form['cc_security']['uc_credit_encryption_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Card number encryption key filepath'),
    '#description' => t('The card type, expiration date and last four digits of the card number are encrypted and stored.<br /><b>You must enable encryption</b> by following the <a href="!url">encryption instructions</a> in order to accept credit card payments.<br />In short, you must specify a path outside of your document root where the encryption key may be stored.<br />Relative paths will be resolved relative to the Drupal installation directory.<br />Once this is set, you should not change it.', array('!url' => 'http://www.ubercart.org/docs/user/14512/credit_card_settings#security')),
    '#default_value' => variable_get('uc_credit_encryption_path', t('Not configured, see below.')),
  );
  $form['cc_security']['uc_credit_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Operate in credit card debug mode.'),
    '#description' => t('In debug mode, the full credit card number is stored which may be in violation of PCI security standards.<br />Debug mode is only recommended for testing transactions with fake credit card details.'),
    '#default_value' => variable_get('uc_credit_debug', FALSE),
  );

  // Form elements to handle the automatic clearing of card data.
  $form['cc_security']['cc_clear'] = array(
    '#type' => 'fieldset',
    '#title' => t('Debug mode data clearing'),
    '#description' => t('Specify below the status and age of orders whose credit card details will be removed.  This setting only applies when operating in debug mode. <strong>Cron must be running for this feature to work.</strong>'),
    '#collapsible' => FALSE,
  );
  foreach (uc_order_status_list() as $status) {
    $options[$status['id']] = $status['title'];
  }
  $form['cc_security']['cc_clear']['uc_credit_clear_status'] = array(
    '#type' => 'select',
    '#title' => t('Order status'),
    '#options' => $options,
    '#default_value' => variable_get('uc_credit_clear_status', uc_order_state_default('completed')),
    '#prefix' => '<div style="float: left; margin-right: 1em;">',
    '#suffix' => '</div>',
  );
  $form['cc_security']['cc_clear']['uc_credit_number_duration'] = array(
    '#type' => 'select',
    '#title' => t('Age'),
    '#options' => drupal_map_assoc(range(1, 24)),
    '#default_value' => variable_get('uc_credit_number_duration', '3'),
    '#prefix' => '<div style="float: left; margin-right: 1em;">',
    '#suffix' => '</div>',
  );
  $form['cc_security']['cc_clear']['uc_credit_number_unit'] = array(
    '#type' => 'select',
    '#title' => t('Unit of time'),
    '#options' => array(
      'hours' => t('hour(s)'),
      'days' => t('day(s)'),
      'weeks' => t('week(s)'),
      'years' => t('year(s)'),
    ),
    '#default_value' => variable_get('uc_credit_number_unit', 'days'),
    '#prefix' => '<div style="float: left;">',
    '#suffix' => '</div>',
  );

  // Form elements that deal with the type of data requested at checkout.
  $form['cc_fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Credit card fields'),
    '#description' => t('Specify what information to collect from customers in addition to the card number.'),
    '#group' => 'uc_credit',
    '#weight' => 10,
  );
  $form['cc_fields']['uc_credit_cvv_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable CVV text field on checkout form.'),
    '#description' => t('The CVV is an added security measure on credit cards. On Visa, Mastercard, and Discover cards it is a three digit number, and on AmEx cards it is a four digit number. If your credit card processor or payment gateway requires this information, you should enable this feature here.'),
    '#default_value' => variable_get('uc_credit_cvv_enabled', TRUE),
  );
  $form['cc_fields']['uc_credit_owner_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable card owner text field on checkout form.'),
    '#default_value' => variable_get('uc_credit_owner_enabled', FALSE),
  );
  $form['cc_fields']['uc_credit_start_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable card start date on checkout form.'),
    '#default_value' => variable_get('uc_credit_start_enabled', FALSE),
  );
  $form['cc_fields']['uc_credit_issue_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable card issue number text field on checkout form.'),
    '#default_value' => variable_get('uc_credit_issue_enabled', FALSE),
  );
  $form['cc_fields']['uc_credit_bank_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable issuing bank text field on checkout form.'),
    '#default_value' => variable_get('uc_credit_bank_enabled', FALSE),
  );
  $form['cc_fields']['uc_credit_type_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable card type selection on checkout form.'),
    '#description' => t('If enabled, specify in the textarea below which card options to populate the select box with.'),
    '#default_value' => variable_get('uc_credit_type_enabled', FALSE),
  );
  $form['cc_fields']['uc_credit_accepted_types'] = array(
    '#type' => 'textarea',
    '#title' => t('Card type select box options'),
    '#description' => t('Enter one card type per line. These fields will populate the card type select box if it is enabled.'),
    '#default_value' => variable_get('uc_credit_accepted_types', implode("\r\n", array(t('Visa'), t('Mastercard'), t('Discover'), t('American Express')))),
  );

  // Form elements that deal with card types accepted.
  $form['cc_fields']['cc_types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Card types'),
    '#description' => t('Use the checkboxes to specify which card types you accept for payment. Selected card types will show their icons in the payment method selection list and be used for card number validation.'),
  );
  $form['cc_fields']['cc_types']['uc_credit_visa'] = array(
    '#type' => 'checkbox',
    '#title' => t('Visa'),
    '#default_value' => variable_get('uc_credit_visa', TRUE),
  );
  $form['cc_fields']['cc_types']['uc_credit_mastercard'] = array(
    '#type' => 'checkbox',
    '#title' => t('Mastercard'),
    '#default_value' => variable_get('uc_credit_mastercard', TRUE),
  );
  $form['cc_fields']['cc_types']['uc_credit_discover'] = array(
    '#type' => 'checkbox',
    '#title' => t('Discover'),
    '#default_value' => variable_get('uc_credit_discover', TRUE),
  );
  $form['cc_fields']['cc_types']['uc_credit_amex'] = array(
    '#type' => 'checkbox',
    '#title' => t('American Express'),
    '#default_value' => variable_get('uc_credit_amex', TRUE),
  );

  // Form elements that deal with credit card messages to customers.
  $form['cc_messages'] = array(
    '#type' => 'fieldset',
    '#title' => t('Customer messages'),
    '#description' => t('Here you can alter messages displayed to customers using credit cards.'),
    '#collapsible' => FALSE,
    '#group' => 'uc_credit',
    '#weight' => 10,
  );
  $form['cc_messages']['uc_credit_policy'] = array(
    '#type' => 'textarea',
    '#title' => t('Credit card payment policy'),
    '#description' => t('Instructions for customers on the checkout page above the credit card fields.'),
    '#default_value' => variable_get('uc_credit_policy', t('Your billing information must match the billing address for the credit card entered below or we will be unable to process your payment.')),
    '#rows' => 3,
  );
  $form['cc_messages']['uc_credit_fail_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Card processing failure message'),
    '#description' => t('Error message displayed to customers when an attempted payment fails at checkout.'),
    '#default_value' => variable_get('uc_credit_fail_message', t('We were unable to process your credit card payment. Please verify your details and try again.  If the problem persists, contact us to complete your order.')),
  );

  $txn_types = array(
    UC_CREDIT_AUTH_ONLY => t('Authorization only'),
    UC_CREDIT_AUTH_CAPTURE => t('Authorize and capture immediately'),
    UC_CREDIT_REFERENCE_SET => t('Set a reference only'),
  );

  foreach ($gateways as $id => $gateway) {
    $form['gateways'][$id] = array(
      '#type' => 'fieldset',
      '#title' => $gateway['title'],
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#group' => 'uc_credit',
      '#weight' => 5,
    );
    $form['gateways'][$id]['uc_pg_' . $id . '_enabled'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable this payment gateway for use.'),
      '#default_value' => variable_get('uc_pg_' . $id . '_enabled', TRUE),
      '#weight' => -10,
    );

    // Get the transaction types associated with this gateway.
    $gateway_types = uc_credit_gateway_txn_types($id);
    $options = array();
    foreach ($txn_types as $type => $title) {
      if (in_array($type, $gateway_types)) {
        $options[$type] = $title;
      }
    }
    $form['gateways'][$id]['uc_pg_' . $id . '_cc_txn_type'] = array(
      '#type' => 'radios',
      '#title' => t('Default credit transaction type'),
      '#description' => t('Only available transaction types are listed. The default will be used unless an administrator chooses otherwise through the terminal.'),
      '#options' => $options,
      '#default_value' => variable_get('uc_pg_' . $id . '_cc_txn_type', UC_CREDIT_AUTH_CAPTURE),
      '#weight' => -5,
    );

    if (isset($gateway['settings']) && function_exists($gateway['settings'])) {
      $gateway_settings = $gateway['settings'](array(), $form_state);
      if (is_array($gateway_settings)) {
        $form['gateways'][$id] += $gateway_settings;
      }
    }
  }

  if (empty($_POST) && !uc_credit_encryption_key()) {
    drupal_set_message(t('Credit card security settings must be configured to accept credit card payments.'), 'warning');
  }

  $form['#validate'][] = 'uc_credit_settings_form_validate';
  $form['#submit'][] = 'uc_credit_settings_form_submit';

  return $form;
}

/**
 * Makes sure the encryption key directory in the credit card settings is valid.
 */
function uc_credit_settings_form_validate($form, &$form_state) {
  $dir = variable_get('uc_credit_encryption_path', t('Not configured, see below.'));
  $filename = rtrim($dir, '/\\') . '/uc_credit.key';

  if ($form_state['values']['uc_credit_encryption_path'] !== variable_get('uc_credit_encryption_path', t('Not configured, see below.'))) {
    $dir = rtrim($form_state['values']['uc_credit_encryption_path'], '/\\');

    $_SESSION['update_cc_encrypt_dir'] = TRUE;
    if (!empty($dir) && $dir !== t('Not configured, see below.')) {
      if (!is_dir($dir)) {
        form_set_error('uc_credit_encryption_path', t('You have specified a non-existent directory.'));
        unset($_SESSION['update_cc_encrypt_dir']);
      }
      else {
        $file = @fopen($dir . '/encrypt.test', 'w');
        if ($file === FALSE) {
          form_set_error('uc_credit_encryption_path', t('Cannot write to directory, please verify the directory permissions.'));
          unset($_SESSION['update_cc_encrypt_dir']);
        }
        else {
          if (@fwrite($file, '0123456789') === FALSE) {
            form_set_error('uc_credit_encryption_path', t('Cannot write to directory, please verify the directory permissions.'));
            unset($_SESSION['update_cc_encrypt_dir']);
            fclose($file);
          }
          else {
            fclose($file);
            $file = @fopen($dir . '/encrypt.test', 'r');
            if ($file === FALSE) {
              form_set_error('uc_credit_encryption_path', t('Cannot read from directory, please verify the directory permissions.'));
              unset($_SESSION['update_cc_encrypt_dir']);
            }
            fclose($file);
          }
          unlink($dir . '/encrypt.test');
        }
      }
    }
  }
}

/**
 * Creates the encryption key file if it doesn't already exist.
 */
function uc_credit_settings_form_submit($form, &$form_state) {
  if (isset($_SESSION['update_cc_encrypt_dir']) &&
      $_SESSION['update_cc_encrypt_dir'] === TRUE) {
    $dir = rtrim($form_state['values']['uc_credit_encryption_path'], '/\\');

    if (!empty($dir) && $dir !== t('Not configured, see below.')) {
      if (!file_exists($dir . '/uc_credit.key')) {
        if (!$file = fopen($dir . '/uc_credit.key', 'wb')) {
          drupal_set_message(t('Credit card encryption key file creation failed. Check your filepath settings and directory permissions.'), 'error');
          watchdog('uc_credit', 'Credit card encryption key file creation failed. Check your filepath settings and directory permissions.', array(), WATCHDOG_ERROR);
        }
        else {
          // Replacement key generation suggested by Barry Jaspan for increased security.
          fwrite($file, md5(drupal_get_token(serialize($_REQUEST) . serialize($_SERVER) . REQUEST_TIME)));
          fclose($file);

          drupal_set_message(t('Credit card encryption key file generated. Card data will now be encrypted.'));
          watchdog('uc_credit', 'Credit card encryption key file generated. Card data will now be encrypted.');
        }
      }
    }
  }

  if (!$form_state['values']['uc_credit_checkout_process'] && !$form_state['values']['uc_credit_debug']) {
    drupal_set_message(t('You have chosen to not process credit card payments during checkout, but customer credit card data is not being stored once checkout is completed. This configuration is not recommended for a live site, as you will not be able to collect payment for your orders.'), 'error');
  }
}

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
    '#tree' => TRUE,
    '#prefix' => '<div class="payment-details-credit clearfix">',
    '#suffix' => '</div>',
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

  $form['#attached']['css'][] = drupal_get_path('module', 'uc_payment') . '/uc_payment.css';

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
    $data['cc_data'] = $crypt->encrypt(uc_credit_encryption_key(), base64_encode(serialize($cc_data)));
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
