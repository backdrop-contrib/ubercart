<?php

/**
 * @file
 * Cart menu items.
 */

/**
 * Displays the cart view page.
 *
 * Show the products in the cart with a form to adjust cart contents or go to
 * checkout.
 */
function uc_cart_view() {
  // Failsafe so that this function only works when called with no arguments.
  // This prevents the accidental wiping of the cart_order session variable.
  if (func_num_args() > 0) {
    return MENU_NOT_FOUND;
  }

  // Clear the cart order session variable if it exists.
  if (!empty($_SESSION['cart_order'])) {
    unset($_SESSION['cart_order']);
  }

  // Load the array of shopping cart items.
  $items = uc_cart_get_contents();

  // Display the empty cart page if there are no items in the cart.
  if (empty($items)) {
    return array(
      '#theme' => 'uc_empty_cart',
    );
  }

  $build = array();
  // Load through the cart panes...
  foreach (uc_cart_cart_pane_list($items) as $id => $pane) {
    // If the pane is enabled...
    if ($pane['enabled']) {
      // Add its output to the cart view.
      $build[$id] = $pane['body'];
    }
  }

  // Add a custom cart breadcrumb if specified.
  if (($text = variable_get('uc_cart_breadcrumb_text', '')) !== '') {
    $link = l($text, variable_get('uc_cart_breadcrumb_url', '<front>'));
    drupal_set_breadcrumb(array($link));
  }

  return $build;
}

/**
 * Displays the cart checkout page built of checkout panes from enabled modules.
 */
function uc_cart_checkout() {
  global $user;

  $items = uc_cart_get_contents();
  if (count($items) == 0 || !variable_get('uc_checkout_enabled', TRUE)) {
    drupal_goto('cart');
  }

  if (($min = variable_get('uc_minimum_subtotal', 0)) > 0) {
    $subtotal = 0;
    if (is_array($items) && count($items) > 0) {
      foreach ($items as $item) {
        $data = module_invoke($item->module, 'uc_cart_display', $item);
        if (!empty($data)) {
          $subtotal += $data['#total'];
        }
      }
    }

    if ($subtotal < $min) {
      drupal_set_message(variable_get('uc_minimum_subtotal_text', t('The minimum order subtotal for checkout is @min.', array('@min' => uc_currency_format($min)))), 'error');
      drupal_goto('cart');
    }
  }

  // Send anonymous users to login page when anonymous checkout is disabled.
  if (!$user->uid && !variable_get('uc_checkout_anonymous', TRUE)) {
    drupal_set_message(t('You must login before you can proceed to checkout.'));
    if (variable_get('user_register', 1) != 0) {
      drupal_set_message(t('If you do not have an account yet, you should <a href="!url">register now</a>.', array('!url' => url('user/register', array('query' => drupal_get_destination())))));
    }
    drupal_goto('user', array('query' => drupal_get_destination()));
  }

  $list = _uc_line_item_list();
  foreach ($list as $line_item) {
    if (isset($line_item['callback']) && function_exists($line_item['callback'])) {
      $line_item['callback']('cart-preview', $items);
    }
  }

  drupal_add_js(drupal_get_path('module', 'uc_cart') . '/uc_cart.js');
  $build = drupal_get_form('uc_cart_checkout_form');

  return $build;
}

/**
 * The checkout form built up from the enabled checkout panes.
 *
 * @see uc_cart_checkout_form_validate()
 * @see uc_cart_checkout_form_review()
 * @see uc_cart_checkout_review()
 * @see theme_uc_cart_checkout_form()
 * @ingroup forms
 */
function uc_cart_checkout_form($form, &$form_state) {
  global $user;

  if (isset($form_state['storage']['order'])) {
    $order = $form_state['storage']['order'];
  }
  else {
    if (isset($_SESSION['cart_order'])) {
      $order = uc_order_load($_SESSION['cart_order']);
    }
    else {
      $order = NULL;
    }
  }

  // Check the referer URI to clear order details and prevent identity theft.
  if (uc_referer_check(array('cart/checkout', 'cart/checkout/review'))) {
    if ($order == FALSE || uc_order_status_data($order->order_status, 'state') != 'in_checkout') {
      unset($_SESSION['cart_order']);
      $order = NULL;
    }
    elseif (uc_order_status_data($order->order_status, 'state') != 'in_checkout' || ($user->uid > 0 && $user->uid != $order->uid)) {
      $order = NULL;
    }
  }
  else {
    unset($_SESSION['cart_order']);
    $order = NULL;
  }

  if (is_null($order)) {
    $order = new UcOrder();
  }

  $form['panes'] = array('#tree' => TRUE);
  $panes = _uc_checkout_pane_list();

  // If the cart isn't shippable, remove panes with shippable == TRUE.
  if (!uc_cart_is_shippable() && variable_get('uc_cart_delivery_not_shippable', TRUE)) {
    $panes = uc_cart_filter_checkout_panes($panes, array('shippable' => TRUE));
  }

  foreach ($panes as $id => $pane) {
    if ($pane['enabled']) {
      $pane['prev'] = _uc_cart_checkout_prev_pane($panes, $id);
      $pane['next'] = _uc_cart_checkout_next_pane($panes, $id);

      if (!isset($pane['collapsed'])) {
        $collapsed = ($pane['prev'] === FALSE || empty($displayed[$pane['prev']])) ? FALSE : TRUE;
      }
      if (isset($form_state['expanded_panes']) && in_array($id, $form_state['expanded_panes'])) {
        $collapsed = FALSE;
      }

      $return = $pane['callback']('view', $order, $form, $form_state);

      // Add the pane if any display data is returned from the callback.
      if (is_array($return) && (!empty($return['description']) || !empty($return['contents']))) {
        // Create the fieldset for the pane.
        $form['panes'][$id] = array(
          '#type' => 'fieldset',
          '#title' => $pane['title'],
          '#description' => !empty($return['description']) ? $return['description'] : NULL,
          '#collapsible' => $pane['collapsible'],
          '#collapsed' => variable_get('uc_use_next_buttons', FALSE) ? $collapsed : FALSE,
          '#id' => $id . '-pane',
          '#theme' => isset($return['theme']) ? $return['theme'] : NULL,
        );

        // Add the contents of the fieldset if any were returned.
        if (!empty($return['contents'])) {
          $form['panes'][$id] = array_merge($form['panes'][$id], $return['contents']);
        }

        // Add the 'Next' button if necessary.
        if ((!isset($return['next-button']) || $return['next-button'] !== FALSE) && $pane['next'] !== FALSE &&
            variable_get('uc_use_next_buttons', FALSE) != FALSE) {
          $opt = variable_get('uc_collapse_current_pane', FALSE) ? $id : 'false';
          $form['panes'][$id]['next'] = array(
            '#type' => 'button',
            '#value' => t('Next'),
            '#weight' => 20,
            '#attributes' => array('onclick' => "return uc_cart_next_button_click(this, '" . $pane['next'] . "', '" . $opt . "');"),
            '#prefix' => '<div class="next-button">',
            '#suffix' => '</div>',
          );
        }

        // Log that this pane was actually displayed.
        $displayed[$id] = TRUE;
      }
    }
  }
  unset($form_state['expanded_panes']);

  $contents = uc_cart_get_contents();

  $form['cart_contents'] = array(
    '#type' => 'hidden',
    '#value' => serialize($contents),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#validate' => array(), // Disable validation to prevent a new order from being created.
    '#limit_validation_errors' => array(),
    '#submit' => array('uc_cart_checkout_form_cancel'),
  );
  $form['actions']['continue'] = array(
    '#type' => 'submit',
    '#value' => t('Review order'),
  );

  unset($_SESSION['do_review']);

  return $form;
}

/**
 * Adds markup and styling to the checkout panes.
 *
 * @see uc_cart_checkout_form()
 * @ingroup themeable
 */
function theme_uc_cart_checkout_form($variables) {
  $form = $variables['form'];

  drupal_add_css(drupal_get_path('module', 'uc_cart') . '/uc_cart.css');

  $output = '<div id="checkout-instructions">' . filter_xss_admin(variable_get('uc_checkout_instructions', '')) . '</div>';

  foreach (element_children($form['panes']) as $pane_id) {
    if (function_exists(($func = _uc_checkout_pane_data($pane_id, 'callback')))) {
      $result = $func('theme', $form['panes'][$pane_id], NULL);
      if (!empty($result)) {
        $output .= $result;
        $form['panes'][$pane_id] = array();
      }
      else {
        $output .= drupal_render($form['panes'][$pane_id]);
      }
    }
    else {
      $output .= drupal_render($form['panes'][$pane_id]);
    }
  }

  $output .= '<div id="checkout-form-bottom">' . drupal_render_children($form) . '</div>';

  return $output;
}

/**
 * Form validation for uc_cart_checkout_form().
 *
 * @see uc_cart_checkout_form()
 * @see uc_cart_checkout_form_submit()
 */
function uc_cart_checkout_form_validate($form, &$form_state) {
  global $user;

  if (empty($form_state['storage']['order'])) {
    $order = uc_order_new($user->uid);
    $form_state['storage']['order'] = $order;
  }
  else {
    $order = $form_state['storage']['order'];
  }
  $_SESSION['cart_order'] = $order->order_id;

  db_delete('uc_order_products')
    ->condition('order_id', $order->order_id)
    ->execute();
  $order->products = unserialize($form_state['values']['cart_contents']);

  $order->order_total = uc_order_get_total($order, TRUE);

  // Validate/process the cart panes.  A FALSE value results in failed checkout.
  $form_state['checkout_valid'] = TRUE;
  foreach (element_children($form_state['values']['panes']) as $pane_id) {
    $func = _uc_checkout_pane_data($pane_id, 'callback');
    if (is_string($func) && function_exists($func)) {
      $isvalid = $func('process', $order, $form, $form_state);
      if ($isvalid === FALSE) {
        $form_state['expanded_panes'][] = $pane_id;
        $form_state['checkout_valid'] = FALSE;
      }
    }
  }

  $order->line_items = uc_order_load_line_items($order, TRUE);
  $order->line_items = array_merge($order->line_items, uc_order_load_line_items($order, FALSE));
  usort($order->line_items, 'uc_weight_sort');

  uc_order_save($order);

  $form_state['storage']['order'] = uc_order_load($order->order_id, TRUE);
}

/**
 * Form submission handler for uc_cart_checkout_form().
 *
 * @see uc_cart_checkout_form()
 * @see uc_cart_checkout_form_validate()
 */
function uc_cart_checkout_form_submit($form, &$form_state) {
  if ($form_state['checkout_valid'] === FALSE) {
    $url = 'cart/checkout';
  }
  else {
    $url = 'cart/checkout/review';
    $_SESSION['do_review'] = TRUE;
  }

  unset($form_state['checkout_valid']);

  $form_state['redirect'] = $url;
}

/**
 * @see uc_checkout_form()
 */
function uc_cart_checkout_form_cancel($form, &$form_state) {
  if (isset($_SESSION['cart_order']) && intval($_SESSION['cart_order']) > 0) {
    uc_order_comment_save($_SESSION['cart_order'], 0, t('Customer cancelled this order from the checkout form.'));
    unset($_SESSION['cart_order']);
  }

  $form_state['redirect'] = 'cart';
}

/**
 * Allows a customer to review their order before finally submitting it.
 *
 * @see uc_cart_checkout_form()
 */
function uc_cart_checkout_review() {
  drupal_add_js(drupal_get_path('module', 'uc_cart') . '/uc_cart.js');

  if (empty($_SESSION['do_review'])) {
    drupal_goto('cart/checkout');
  }

  $order = uc_order_load($_SESSION['cart_order']);

  if ($order === FALSE || uc_order_status_data($order->order_status, 'state') != 'in_checkout') {
    unset($_SESSION['cart_order']);
    drupal_goto('cart/checkout');
  }

  $panes = _uc_checkout_pane_list();

  // If the cart isn't shippable, bypass panes with shippable == TRUE.
  if (!uc_cart_is_shippable() && variable_get('uc_cart_delivery_not_shippable', TRUE)) {
    $panes = uc_cart_filter_checkout_panes($panes, array('shippable' => TRUE));
  }

  foreach ($panes as $pane) {
    if ($pane['enabled']) {
      $func = $pane['callback'];
      if (function_exists($func)) {
        $return = $func('review', $order, NULL);
        if (!is_null($return)) {
          $data[$pane['title']] = $return;
        }
      }
    }
  }

  $build = array(
    '#theme' => 'uc_cart_checkout_review',
    '#panes' => $data,
    '#form' => drupal_get_form('uc_cart_checkout_review_form', $order),
  );

  return $build;
}

/**
 * Themes the checkout review order page.
 *
 * @param $panes
 *   An associative array for each checkout pane that has information to add to
 *   the review page.  The key is the pane's title and the value is either the
 *   data returned for that pane or an array of returned data.
 * @param $form
 *   The HTML version of the form that by default includes the 'Back' and
 *   'Submit order' buttons at the bottom of the review page.
 *
 * @return
 *   A string of HTML for the page contents.
 *
 * @ingroup themeable
 */
function theme_uc_cart_checkout_review($variables) {
  $panes = $variables['panes'];
  $form = $variables['form'];

  drupal_add_css(drupal_get_path('module', 'uc_cart') . '/uc_cart.css');

  $output = '<div id="review-instructions">' . filter_xss_admin(variable_get('uc_checkout_review_instructions', uc_get_message('review_instructions'))) . '</div>';

  $output .= '<table class="order-review-table">';

  foreach ($panes as $title => $data) {
    $output .= '<tr class="pane-title-row"><td colspan="2">' . $title
              . '</td></tr>';
    if (is_array($data)) {
      foreach ($data as $row) {
        if (is_array($row)) {
          if (isset($row['border'])) {
            $border = ' class="row-border-' . $row['border'] . '"';
          }
          else {
            $border = '';
          }
          $output .= '<tr valign="top"' . $border . '><td class="title-col" '
                    . 'nowrap>' . $row['title'] . ':</td><td class="data-col">'
                   . $row['data'] . '</td></tr>';
        }
        else {
          $output .= '<tr valign="top"><td colspan="2">' . $row . '</td></tr>';
        }
      }
    }
    else {
      $output .= '<tr valign="top"><td colspan="2">' . $data . '</td></tr>';
    }
  }

  $output .= '<tr class="review-button-row"><td colspan="2">' . drupal_render($form)
            . '</td></tr></table>';

  return $output;
}

/**
 * Gives customers the option to finish checkout or go revise their information.
 *
 * @see uc_cart_checkout_review_form_back()
 * @see uc_cart_checkout_review_form_submit()
 * @ingroup forms
 */
function uc_cart_checkout_review_form($form, &$form_state, $order) {
  $form_state['uc_order'] = $order;

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['back'] = array(
    '#type' => 'submit',
    '#value' => t('Back'),
    '#submit' => array('uc_cart_checkout_review_form_back'),
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit order'),
  );

  return $form;
}

/**
 * Returns the customer to the checkout page to edit their information.
 *
 * @see uc_cart_checkout_review_form()
 */
function uc_cart_checkout_review_form_back($form, &$form_state) {
  unset($_SESSION['do_review']);
  $form_state['redirect'] = 'cart/checkout';
}

/**
 * Final checks to make sure the order can be completed.
 *
 * @see uc_cart_checkout_review_form()
 */
function uc_cart_checkout_review_form_submit($form, &$form_state) {
  // Invoke hook_uc_order($op = 'submit') to test to make sure the order can
  // be completed... used for auto payment in uc_credit.module.
  $order = uc_order_load($_SESSION['cart_order']);
  $error = FALSE;

  // Invoke it on a per-module basis instead of all at once.
  foreach (module_implements('uc_order') as $module) {
    $function = $module . '_uc_order';
    if (function_exists($function)) {
      // $order must be passed by reference.
      $result = $function('submit', $order, NULL);

      $msg_type = 'status';
      if ($result[0]['pass'] === FALSE) {
        $error = TRUE;
        $msg_type = 'error';
      }
      if (!empty($result[0]['message'])) {
        drupal_set_message($result[0]['message'], $msg_type);
      }

      // Stop invoking the hooks if there was an error.
      if ($error) {
        break;
      }
    }
  }

  if ($error) {
    $form_state['redirect'] = 'cart/checkout/review';
  }
  else {
    unset($_SESSION['do_review']);
    $_SESSION['do_complete'] = TRUE;
    $form_state['redirect'] = 'cart/checkout/complete';
  }
}

/**
 * Completes the sale and finishes checkout.
 */
function uc_cart_checkout_complete() {
  if (!isset($_SESSION['do_complete']) || !$_SESSION['do_complete']) {
    drupal_goto('cart');
  }

  $order = uc_order_load(intval($_SESSION['cart_order']));

  if (empty($order)) {
    // Display messages to customers and the administrator if the order was lost.
    drupal_set_message(t("We're sorry.  An error occurred while processing your order that prevents us from completing it at this time. Please contact us and we will resolve the issue as soon as possible."), 'error');
    watchdog('uc_cart', 'An empty order made it to checkout! Cart order ID: @cart_order', array('@cart_order' => $_SESSION['cart_order']), WATCHDOG_ERROR);
    drupal_goto('cart');
  }

  $build = uc_cart_complete_sale($order, variable_get('uc_new_customer_login', FALSE));

  // Add a comment to let sales team know this came in through the site.
  uc_order_comment_save($order->order_id, 0, t('Order created through website.'), 'admin');

  $page = variable_get('uc_cart_checkout_complete_page', '');
  if (!empty($page)) {
    drupal_goto($page);
  }

  return $build;
}
