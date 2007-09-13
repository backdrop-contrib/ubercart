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
                     . t('You can edit your account information <a href="!url">here</a>.', array('!url' => url('user/'. $user->uid .'/edit')));
      }
      else {
        $description = t('Enter a valid email address for this order or <a href="!url">click here</a> to login with an existing account and return to checkout.', array('!url' => url('user/login')));
      }

      $email = (is_null($arg1) || empty($arg1->primary_email)) ? $user->mail : $arg1->primary_email;
      $contents['primary_email'] = uc_textfield(t('E-mail Address'), $email, TRUE, NULL, 64);
      $contents['primary_email']['#disabled'] = ($user->uid) ? TRUE : FALSE;

      return array('description' => $description, 'contents' => $contents);

    case 'process':
      $arg1->primary_email = $arg2['primary_email'];
      if ($user->uid) {
        $arg1->uid = $user->uid;
      }
      return TRUE;

    case 'review':
      $review[] = array('title' => t('E-mail'), 'data' => $arg1->primary_email);
      return $review;
  }
}

/**
 * Get the delivery information.
 */
function uc_checkout_pane_delivery($op, &$arg1, $arg2) {
  global $user;

  if (!uc_cart_is_shippable() && variable_get('uc_cart_delivery_not_shippable', TRUE)) {
    return;
  }

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
        $contents['delivery_first_name'] = uc_textfield(uc_get_field_name('first_name'), $arg1->delivery_first_name);
      }
      if (uc_address_field_enabled('last_name')) {
        $contents['delivery_last_name'] = uc_textfield(uc_get_field_name('last_name'), $arg1->delivery_last_name);
      }
      if (uc_address_field_enabled('company')) {
        $contents['delivery_company'] = uc_textfield(uc_get_field_name('company'), $arg1->delivery_company, FALSE, NULL, 64);
      }
      if (uc_address_field_enabled('street1')) {
        $contents['delivery_street1'] = uc_textfield(uc_get_field_name('street1'), $arg1->delivery_street1, TRUE, NULL, 64);
      }
      if (uc_address_field_enabled('street2')) {
        $contents['delivery_street2'] = uc_textfield(uc_get_field_name('street2'), $arg1->delivery_street2, FALSE, NULL, 64);
      }
      if (uc_address_field_enabled('city')) {
        $contents['delivery_city'] = uc_textfield(uc_get_field_name('city'), $arg1->delivery_city);
      }
      if (uc_address_field_enabled('country')) {
        $contents['delivery_country'] = uc_country_select(uc_get_field_name('country'), $arg1->delivery_country);
      }
      if (uc_address_field_enabled('zone')) {
        if (isset($_POST['panes']['delivery']['pane']['delivery_country'])) {
          $country_id = intval($_POST['panes']['delivery']['pane']['delivery_country']);
        }
        else {
          $country_id = $arg1->delivery_country;
        }
        $contents['delivery_zone'] = uc_zone_select(uc_get_field_name('zone'), $arg1->delivery_zone, NULL, $country_id);
      }
      if (uc_address_field_enabled('postal_code')) {
        $contents['delivery_postal_code'] = uc_textfield(uc_get_field_name('postal_code'), $arg1->delivery_postal_code, TRUE, NULL, 10, 10);
      }
      if (uc_address_field_enabled('phone')) {
        $contents['delivery_phone'] = uc_textfield(uc_get_field_name('phone'), $arg1->delivery_phone, TRUE, NULL, 32, 16);
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
      if (uc_address_field_enabled('phone')) {
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

      $contents['copy_address'] = array(
        '#type' => 'checkbox',
        '#title' => t('My billing information is the same as my delivery information.'),
        '#attributes' => array('onclick' => 'uc_cart_copy_delivery_to_billing(this.checked);'),
      );

      if ($user->uid) {
        $addresses = uc_select_address($user->uid, 'billing', 'apply_address(\'billing\', this.value);', t('Saved addresses'), TRUE);
        if (!empty($addresses)) {
          $contents['billing_address_select'] = $addresses;
        }
      }
      if (uc_address_field_enabled('first_name')) {
        $contents['billing_first_name'] = uc_textfield(uc_get_field_name('first_name'), $arg1->billing_first_name);
      }
      if (uc_address_field_enabled('last_name')) {
        $contents['billing_last_name'] = uc_textfield(uc_get_field_name('last_name'), $arg1->billing_last_name);
      }
      if (uc_address_field_enabled('company')) {
        $contents['billing_company'] = uc_textfield(uc_get_field_name('company'), $arg1->billing_company, FALSE, NULL, 64);
      }
      if (uc_address_field_enabled('street1')) {
        $contents['billing_street1'] = uc_textfield(uc_get_field_name('street1'), $arg1->billing_street1, TRUE, NULL, 64);
      }
      if (uc_address_field_enabled('street2')) {
        $contents['billing_street2'] = uc_textfield(uc_get_field_name('street2'), $arg1->billing_street2, FALSE, NULL, 64);
      }
      if (uc_address_field_enabled('city')) {
        $contents['billing_city'] = uc_textfield(uc_get_field_name('city'), $arg1->billing_city);
      }
      if (uc_address_field_enabled('country')) {
        $contents['billing_country'] = uc_country_select(uc_get_field_name('country'), $arg1->billing_country);
      }
      if (uc_address_field_enabled('zone')) {
        if (isset($_POST['panes']['billing']['pane']['billing_country'])) {
          $country_id = intval($_POST['panes']['billing']['pane']['billing_country']);
        }
        else {
          $country_id = $arg1->billing_country;
        }
        $contents['billing_zone'] = uc_zone_select(uc_get_field_name('zone'), $arg1->billing_zone, NULL, $country_id);
      }
      if (uc_address_field_enabled('postal_code')) {
        $contents['billing_postal_code'] = uc_textfield(uc_get_field_name('postal_code'), $arg1->billing_postal_code, TRUE, NULL, 10, 10);
      }
      if (uc_address_field_enabled('phone')) {
        $contents['billing_phone'] = uc_textfield(uc_get_field_name('phone'), $arg1->billing_phone, TRUE, NULL, 32, 16);
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
      if (uc_address_field_enabled('phone')) {
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

      $result = db_query("SELECT message FROM {uc_order_comments} WHERE "
                        ."order_id = %d", $arg1->order_id);
      if ($comment = db_fetch_object($result)) {
        $default = $comment->message;
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
      if (substr($field, 9) == 'street1') {
        $title = uc_get_field_name('street') .':';
      }
      elseif (substr($field, 9) == 'street2') {
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
function _uc_cart_checkout_prev_pane($pane_id = NULL) {
  if (is_null($pane_id)) {
    return FALSE;
  }

  $panes = _checkout_pane_list();

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
function _uc_cart_checkout_next_pane($pane_id = NULL) {
  if (is_null($pane_id)) {
    return FALSE;
  }

  $panes = _checkout_pane_list();

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

