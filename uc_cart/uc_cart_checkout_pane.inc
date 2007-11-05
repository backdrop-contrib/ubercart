<?php
// $Id$

/**
 * @file
 * This file contains the callbacks for the default checkout panes supplied with
 * Ubercart and their corresponding helper functions.
 *
 * Checkout panes are defined using hook_checkout_pane() and use a callback to
 * handle the different processes involved in completing the checkout form.  The
 * default checkout panes are defined in uc_cart_checkout_pane() in
 * uc_cart.module.
 */

/**
 * Display the cart contents for review during checkout.
 */
function uc_checkout_pane_cart($op) {
  switch ($op) {
    case 'view':
      $contents['cart_review_table'] = array(
        '#value' => theme('cart_review_table'),
        '#weight' => variable_get('uc_pane_cart_field_cart_weight', 2),
      );
      return array('contents' => $contents, 'next-button' => FALSE);

    case 'review':
      $items = uc_cart_get_contents();
      $output = '<table>';
      foreach ($items as $item) {
        $rows = array();
        foreach($item->options as $option){
          $rows[] = $option['attribute'] .': '. $option['name'];
        }
        $desc = $item->title . theme('item_list', $rows, NULL, 'ul', array('class' => 'product-options'));
        $output .= '<tr valign="top"><td>'. $item->qty .'x</td><td width="100%">'. $desc
                  .'</td><td nowrap="nowrap">'. uc_currency_format($item->price * $item->qty) .'</td></tr>';
      }
      $output .= '</table>';
      $review[] = $output;
      return $review;
  }
}

/**
 * Get the user's email address for login.
 */
function uc_checkout_pane_customer($op, &$arg1, $arg2) {
  global $user;

  switch ($op) {
    case 'view':
      if ($user->uid) {
        $description = t('Order information will be sent to your account email listed below.') .'<br />'
                     . t('You can edit your account information <a href="!url">here</a>.', array('!url' => url('user/'. $user->uid .'/edit', 'destination=cart/checkout')));
      }
      else {
        $description = t('Enter a valid email address for this order or <a href="!url">click here</a> to login with an existing account and return to checkout.', array('!url' => url('user/login')));
      }

      $email = (is_null($arg1) || empty($arg1->primary_email)) ? $user->mail : $arg1->primary_email;
      $contents['primary_email'] = uc_textfield(t('E-mail Address'), $email, TRUE, NULL, 64);
      $contents['primary_email']['#disabled'] = ($user->uid) ? TRUE : FALSE;

      if (variable_get('uc_cart_email_validation', TRUE) && !$user->uid) {
        $contents['primary_email_confirm'] = uc_textfield(t('Confirm E-mail Address'), '', TRUE, NULL, 64);
      }

      if ((variable_get('uc_cart_new_account_name', TRUE) ||
          variable_get('uc_cart_new_account_password', TRUE)) &&
          $user->uid == 0) {
        $contents['new_account'] = array(
          '#type' => 'fieldset',
          '#title' => t('New account details'),
          '#description' => variable_get('uc_cart_new_account_details', t('<b>Optional.</b> New customers may supply custom account details.<br />We will create these for you if no values are entered.')),
          '#collapsible' => FALSE,
        );

        if (variable_get('uc_cart_new_account_name', TRUE)) {
          $contents['new_account']['name'] = array(
            '#type' => 'textfield',
            '#title' => t('Username'),
            '#maxlength' => 60,
            '#size' => 32,
          );
        }
        if (variable_get('uc_cart_new_account_password', TRUE)) {
          $contents['new_account']['pass'] = array(
            '#type' => 'password',
            '#title' => t('Password'),
            '#default_value' => $_SESSION['new_user']['pass'],
            '#maxlength' => 32,
            '#size' => 32,
          );
          $contents['new_account']['pass_confirm'] = array(
            '#type' => 'password',
            '#title' => t('Confirm password'),
            '#description' => t('Passwords must match to proceed.'),
            '#default_value' => $_SESSION['new_user']['pass_confirm'],
            '#maxlength' => 32,
            '#size' => 32,
          );
        }
      }

      return array('description' => $description, 'contents' => $contents);

    case 'process':
      if (!valid_email_address($arg2['primary_email'])) {
        drupal_set_message(t('You must enter a valid e-mail address.'), 'error');
        return FALSE;
      }

      $arg1->primary_email = $arg2['primary_email'];

      if (variable_get('uc_cart_email_validation', TRUE) && !$user->uid &&
          $arg2['primary_email'] !== $arg2['primary_email_confirm']) {
        drupal_set_message(t('The e-mail address did not match.'), 'error');
        return FALSE;
      }

      // If new users can specify names or passwords then...
      if ((variable_get('uc_cart_new_account_name', TRUE) ||
          variable_get('uc_cart_new_account_password', TRUE)) &&
          $user->uid == 0) {
        // Skip if an account already exists for this e-mail address.
        if (db_num_rows(db_query("SELECT uid FROM {users} WHERE LOWER(mail) = LOWER('%s')", $arg2['primary_email'])) > 0) {
          drupal_set_message(t('An account already exists for your e-mail address. The new account details you entered will be disregarded.'));
        }
        else {
          // Validate the username.
          if (variable_get('uc_cart_new_account_name', TRUE) && !empty($arg2['new_account']['name'])) {
            $message = user_validate_name($arg2['new_account']['name']);
            if (!empty($message)) {
              drupal_set_message($message, 'error');
              return FALSE;
            }
            if (db_num_rows(db_query("SELECT uid FROM {users} WHERE LOWER(name) = LOWER('%s')", $arg2['new_account']['name'])) > 0) {
              drupal_set_message(t('The username %name is already taken. Please enter a different name or leave the field blank for your username to be your e-mail address.', array('%name' => $arg2['new_account']['name'])), 'error');
              return FALSE;
            }
            $_SESSION['new_user']['name'] = $arg2['new_account']['name'];
          }
          // Validate the password.
          if (variable_get('uc_cart_new_account_pass', TRUE)) {
            if ($arg2['new_account']['pass'] != $arg2['new_account']['pass_confirm']) {
              drupal_set_message(t('The passwords you entered did not match. Please try again.'), 'error');
              return FALSE;
            }
            $_SESSION['new_user']['pass'] = $arg2['new_account']['pass'];
          }
        }
      }

      if ($user->uid) {
        $arg1->uid = $user->uid;
      }
      return TRUE;

    case 'review':
      $review[] = array('title' => t('E-mail'), 'data' => $arg1->primary_email);
      return $review;

    case 'settings':
      $form['uc_cart_email_validation'] = array(
        '#type' => 'checkbox',
        '#title' => t('Require e-mail validation for anonymous customers.'),
        '#default_value' => variable_get('uc_cart_email_validation', FALSE),
      );
      $form['uc_cart_new_account_name'] = array(
        '#type' => 'checkbox',
        '#title' => t('Allow anonymous customers to specify a new user acount name.'),
        '#default_value' => variable_get('uc_cart_new_account_name', FALSE),
      );
      $form['uc_cart_new_account_password'] = array(
        '#type' => 'checkbox',
        '#title' => t('Allow anonymous customers to specify a new user acount password.'),
        '#default_value' => variable_get('uc_cart_new_account_password', FALSE),
      );
      $form['uc_cart_new_account_details'] = array(
        '#type' => 'textarea',
        '#title' => t('New account details help message'),
        '#description' => t('Enter the help message displayed in the new account details fieldset when shown.'),
        '#default_value' => variable_get('uc_cart_new_account_details', t('<b>Optional.</b> New customers may supply custom account details.<br />We will create these for you if no values are entered.')),
      );
      return $form;
  }
}

/**
 * Get the delivery information.
 */
function uc_checkout_pane_delivery($op, &$arg1, $arg2) {
  global $user;

  switch ($op) {
    case 'view':
      $description = t('Enter your delivery address and information here.');

      if ($user->uid) {
        $addresses = uc_select_address($user->uid, 'delivery', 'apply_address(\'delivery\', this.value);', t('Saved addresses'), TRUE);
        if (!empty($addresses)) {
          $contents['delivery_address_select'] = $addresses;
        }
      }

      if (uc_address_field_enabled('first_name')) {
        $contents['delivery_first_name'] = uc_textfield(uc_get_field_name('first_name'), $arg1->delivery_first_name, uc_address_field_required('first_name'));
      }
      if (uc_address_field_enabled('last_name')) {
        $contents['delivery_last_name'] = uc_textfield(uc_get_field_name('last_name'), $arg1->delivery_last_name, uc_address_field_required('last_name'));
      }
      if (uc_address_field_enabled('company')) {
        $contents['delivery_company'] = uc_textfield(uc_get_field_name('company'), $arg1->delivery_company, uc_address_field_required('company'), NULL, 64);
      }
      if (uc_address_field_enabled('street1')) {
        $contents['delivery_street1'] = uc_textfield(uc_get_field_name('street1'), $arg1->delivery_street1, uc_address_field_required('street1'), NULL, 64);
      }
      if (uc_address_field_enabled('street2')) {
        $contents['delivery_street2'] = uc_textfield(uc_get_field_name('street2'), $arg1->delivery_street2, uc_address_field_required('street2'), NULL, 64);
      }                                                                                                                        
      if (uc_address_field_enabled('city')) {
        $contents['delivery_city'] = uc_textfield(uc_get_field_name('city'), $arg1->delivery_city, uc_address_field_required('city'));
      }
      if (uc_address_field_enabled('country')) {
        $contents['delivery_country'] = uc_country_select(uc_get_field_name('country'), $arg1->delivery_country, NULL, 'name', uc_address_field_required('country'));
      }
      if (uc_address_field_enabled('zone')) {
        if (isset($_POST['panes']['delivery']['delivery_country'])) {
          $country_id = intval($_POST['panes']['delivery']['delivery_country']);
        }
        else {
          $country_id = $arg1->delivery_country;
        }
        $contents['delivery_zone'] = uc_zone_select(uc_get_field_name('zone'), $arg1->delivery_zone, NULL, $country_id, 'name', uc_address_field_required('zone'));
      }
      if (uc_address_field_enabled('postal_code')) {
        $contents['delivery_postal_code'] = uc_textfield(uc_get_field_name('postal_code'), $arg1->delivery_postal_code, uc_address_field_required('postal_code'), NULL, 10, 10);
      }
      if (uc_address_field_enabled('phone')) {
        $contents['delivery_phone'] = uc_textfield(uc_get_field_name('phone'), $arg1->delivery_phone, uc_address_field_required('phone'), NULL, 32, 16);
      }

      return array('description' => $description, 'contents' => $contents, 'theme' => 'address_pane');

    case 'process':
      $arg1->delivery_first_name = $arg2['delivery_first_name'];
      $arg1->delivery_last_name = $arg2['delivery_last_name'];
      $arg1->delivery_company = $arg2['delivery_company'];
      $arg1->delivery_street1 = $arg2['delivery_street1'];
      $arg1->delivery_street2 = $arg2['delivery_street2'];
      $arg1->delivery_city = $arg2['delivery_city'];
      $arg1->delivery_zone = $arg2['delivery_zone'];
      $arg1->delivery_postal_code = $arg2['delivery_postal_code'];
      $arg1->delivery_country = $arg2['delivery_country'];
      $arg1->delivery_phone = $arg2['delivery_phone'];
      return TRUE;

    case 'review':
      $review[] = array('title' => t('Address'), 'data' => uc_order_address($arg1, 'delivery', FALSE));
      if (uc_address_field_enabled('phone') && !empty($arg1->delivery_phone)) {
        $review[] = array('title' => t('Phone'), 'data' => $arg1->delivery_phone);
      }
      return $review;
  }
}

/**
 * Get the billing information.
 */
function uc_checkout_pane_billing($op, &$arg1, $arg2) {
  global $user;

  switch ($op) {
    case 'view':
      $description = t('Enter your billing address and information here.');

      if (uc_cart_is_shippable() || !variable_get('uc_cart_delivery_not_shippable', TRUE)) {
        $contents['copy_address'] = array(
          '#type' => 'checkbox',
          '#title' => t('My billing information is the same as my delivery information.'),
          '#attributes' => array('onclick' => 'uc_cart_copy_delivery_to_billing(this.checked);'),
        );
      }

      if ($user->uid) {
        $addresses = uc_select_address($user->uid, 'billing', 'apply_address(\'billing\', this.value);', t('Saved addresses'), TRUE);
        if (!empty($addresses)) {
          $contents['billing_address_select'] = $addresses;
        }
      }
      if (uc_address_field_enabled('first_name')) {
        $contents['billing_first_name'] = uc_textfield(uc_get_field_name('first_name'), $arg1->billing_first_name, uc_address_field_required('first_name'));
      }
      if (uc_address_field_enabled('last_name')) {
        $contents['billing_last_name'] = uc_textfield(uc_get_field_name('last_name'), $arg1->billing_last_name, uc_address_field_required('last_name'));
      }
      if (uc_address_field_enabled('company')) {
        $contents['billing_company'] = uc_textfield(uc_get_field_name('company'), $arg1->billing_company, uc_address_field_required('company'), NULL, 64);
      }
      if (uc_address_field_enabled('street1')) {
        $contents['billing_street1'] = uc_textfield(uc_get_field_name('street1'), $arg1->billing_street1, uc_address_field_required('street1'), NULL, 64);
      }
      if (uc_address_field_enabled('street2')) {
        $contents['billing_street2'] = uc_textfield(uc_get_field_name('street2'), $arg1->billing_street2, uc_address_field_required('street2'), NULL, 64);
      }
      if (uc_address_field_enabled('city')) {
        $contents['billing_city'] = uc_textfield(uc_get_field_name('city'), $arg1->billing_city, uc_address_field_required('city'));
      }
      if (uc_address_field_enabled('country')) {
        $contents['billing_country'] = uc_country_select(uc_get_field_name('country'), $arg1->billing_country, NULL, 'name', uc_address_field_required('country'));
      }
      if (uc_address_field_enabled('zone')) {
        if (isset($_POST['panes']['billing']['billing_country'])) {
          $country_id = intval($_POST['panes']['billing']['billing_country']);
        }
        else {
          $country_id = $arg1->billing_country;
        }
        $contents['billing_zone'] = uc_zone_select(uc_get_field_name('zone'), $arg1->billing_zone, NULL, $country_id, 'name', uc_address_field_required('zone'));
      }
      if (uc_address_field_enabled('postal_code')) {
        $contents['billing_postal_code'] = uc_textfield(uc_get_field_name('postal_code'), $arg1->billing_postal_code, uc_address_field_required('postal_code'), NULL, 10, 10);
      }
      if (uc_address_field_enabled('phone')) {
        $contents['billing_phone'] = uc_textfield(uc_get_field_name('phone'), $arg1->billing_phone, uc_address_field_required('phone'), NULL, 32, 16);
      }

      return array('description' => $description, 'contents' => $contents, 'theme' => 'address_pane');

    case 'process':
      $arg1->billing_first_name = $arg2['billing_first_name'];
      $arg1->billing_last_name = $arg2['billing_last_name'];
      $arg1->billing_company = $arg2['billing_company'];
      $arg1->billing_street1 = $arg2['billing_street1'];
      $arg1->billing_street2 = $arg2['billing_street2'];
      $arg1->billing_city = $arg2['billing_city'];
      $arg1->billing_zone = $arg2['billing_zone'];
      $arg1->billing_postal_code = $arg2['billing_postal_code'];
      $arg1->billing_country = $arg2['billing_country'];
      $arg1->billing_phone = $arg2['billing_phone'];
      return TRUE;

    case 'review':
      $review[] = array('title' => t('Address'), 'data' => uc_order_address($arg1, 'billing', FALSE));
      if (uc_address_field_enabled('phone') && !empty($arg1->billing_phone)) {
        $review[] = array('title' => t('Phone'), 'data' => $arg1->billing_phone);
      }
      return $review;
  }
}

/**
 * Allow a customer to make comments on the order.
 */
function uc_checkout_pane_comments($op, &$arg1, $arg2) {
  switch ($op) {
    case 'view':
      $description = t('Use this area for special instructions or questions regarding your order.');

      if (!empty($arg1->order_id)) {
        $default = db_result(db_query("SELECT message FROM {uc_order_comments} WHERE order_id = %d", $arg1->order_id));
      }
      $contents['comments'] = array(
        '#type' => 'textarea',
        '#title' => t('Order Comments'),
        '#default_value' => $default,
      );

      return array('description' => $description, 'contents' => $contents);

    case 'process':
      if (strlen($arg2['comments']) > 0) {
        db_query("DELETE FROM {uc_order_comments} WHERE order_id = %d", $arg1->order_id);
        uc_order_comment_save($arg1->order_id, 0, $arg2['comments'], 'order', 1, TRUE);
      }
      return TRUE;

    case 'review':
      $result = db_query("SELECT message FROM {uc_order_comments} WHERE "
                        ."order_id = %d", $arg1->order_id);
      if ($comment = db_fetch_object($result)) {
        $review[] = array('title' => t('Comment'), 'data' => $comment->message);
      }
      return $review;
  }
}

// Theme the delivery/billing address forms in tables.
function theme_address_pane($form) {
  $req = '<span class="form-required">*</span>';

  if (isset($form['copy_address'])) {
    $output = drupal_render($form['copy_address']);
  }

  $output .= '<div class="address-pane-table"><table>';

  foreach (element_children($form) as $field) {
    if (substr($field, 0, 9) == 'delivery_' || substr($field, 0, 8) == 'billing_') {
      $title = $form[$field]['#title'] .':';
      unset($form[$field]['#title']);
      if (substr($field, -7) == 'street1') {
        $title = uc_get_field_name('street') .':';
      }
      elseif (substr($field, -7) == 'street2') {
        $title = ' ';
      }
      $output .= '<tr><td class="field-label">';
      if ($form[$field]['#required']) {
        $output .= $req;
      }
      $output .= $title .'</td><td>'. drupal_render($form[$field]) .'</td></tr>';
    }
  }
  $output .= '</table></div>';

  foreach (element_children($form) as $element) {
    $output .= drupal_render($form[$element]);
  }

  return $output;
}

/**
 * Find the collapsible pane displayed above the pane with an ID of $pane_id.
 */
function _uc_cart_checkout_prev_pane($panes, $pane_id = NULL) {
  if (is_null($pane_id)) {
    return FALSE;
  }

  $prev = FALSE;
  foreach ($panes as $target) {
    if ($target['id'] == $pane_id) {
      return $prev;
    }
    if ($target['collapsible'] && variable_get('uc_pane_'. $target['id'] .'_enabled', TRUE)) {
      $prev = $target['id'];
    }
  }

  return FALSE;
}

/**
 * Find the pane that displays below the pane with an ID of $pane_id.
 */
function _uc_cart_checkout_next_pane($panes, $pane_id = NULL) {
  if (is_null($pane_id)) {
    return FALSE;
  }

  $next = FALSE;
  foreach ($panes as $target) {
    if ($next) {
      if ($target['collapsible'] && variable_get('uc_pane_'. $target['id'] .'_enabled', TRUE)) {
        return $target['id'];
      }
    }
    if ($target['id'] == $pane_id) {
      $next = TRUE;
    }
  }

  return FALSE;
}

/**
 * Build a list of checkout panes defined in the enabled modules.
 */
function _checkout_pane_list($action = NULL) {
  static $panes;

  if (count($panes) > 0 && $action !== 'rebuild') {
    return $panes;
  }

  $panes = module_invoke_all('checkout_pane', NULL);
  for ($i = 0; $i < count($panes); $i++) {
    $panes[$i]['enabled'] = variable_get('uc_pane_'. $panes[$i]['id'] .'_enabled', (!isset($panes[$i]['enabled']) ? TRUE : $panes[$i]['enabled']));
    $panes[$i]['weight'] = variable_get('uc_pane_'. $panes[$i]['id'] .'_weight', (!isset($panes[$i]['weight']) ? 0 : $panes[$i]['weight']));
    $panes[$i]['review'] = !isset($panes[$i]['review']) ? TRUE : $panes[$i]['review'];
    $panes[$i]['process'] = !isset($panes[$i]['process']) ? TRUE : $panes[$i]['process'];
    $panes[$i]['collapsible'] = !isset($panes[$i]['collapsible']) ? TRUE : $panes[$i]['collapsible'];
  }
  usort($panes, 'uc_weight_sort');

  return $panes;
}

/**
 * Return data from a checkout pane by pane ID and the array key.
 */
function _checkout_pane_data($pane_id, $key) {
  $panes = _checkout_pane_list();
  foreach ($panes as $pane) {
    if ($pane['id'] == $pane_id) {
      return $pane[$key];
    }
  }
}

function theme_cart_review_table($show_subtotal = TRUE) {
  $items = uc_cart_get_contents();
  $subtotal = 0;

  $output = '<table class="cart-review"><thead>'
           .'<tr class="first last odd"><td class="first odd qty">'. t('Qty')
           .'</td><td class="even products">'. t('Products')
           .'</td><td class="last odd price">'. t('Price')
           .'</td></tr></thead><tbody>';

  $row = 1;
  for ($i = 0; $i < count($items); $i++) {
    $item = $items[$i];

    $rows = array();
    foreach($item->options as $option){
      $rows[] = $option['attribute'] .': '. $option['name'];
    }
    $desc = $item->title . theme('item_list', $rows, NULL, 'ul', array('class' => 'product-options'));

    $total = ($item->qty) ? $item->qty * $item->price : $item->price;
    $subtotal += $total;
    $qty = ($item->qty) ? $item->qty : '';
    $tr_class = ($i % 2 == 0) ? 'even' : 'odd';
    if ($show_subtotal && $i == count($items)) {
      $tr_class .= ' last';
    }

    $output .= '<tr class="'. $tr_class .'"><td class="qty">'
             . t('!qtyx', array('!qty' => $qty)) .'</td><td class="products">'
             . $desc .'</td><td class="price">'. uc_currency_format($total)
              .'</td></tr>';
  }
  if ($show_subtotal) {
    $tr_class = ($tr_class == 'even') ? 'odd' : 'even';
    $output .= '<tr class="'. $tr_class .' last"><td class="subtotal" '
              .'colspan="4"><span id="subtotal-title">'. t('Subtotal:')
              .'</span> '. uc_currency_format($subtotal) .'</td></tr>';
  }
  $output .= '</tbody></table>';

  return $output;
}

