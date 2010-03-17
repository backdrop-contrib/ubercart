<?php
// $Id$

/**
 * @file
 * Cart links menu items.
 */

/**
 * Process a cart link to fiddle with the cart and redirect the user.
 *
 * @param $arg1
 *   An action.
 */
function uc_cart_links_process($arg1) {
  $messages = array();

  // Fail if the link is restricted.
  $data = variable_get('uc_cart_links_restrictions', '');
  if (!empty($data)) {
    $restrictions = explode("\n", variable_get('uc_cart_links_restrictions', ''));
    if (!empty($restrictions) && !in_array($arg1, $restrictions)) {
      $url = variable_get('uc_cart_links_invalid_page', '');
      if (empty($url)) {
        $url = '<front>';
      }
      unset($_REQUEST['destination']);
      drupal_goto($url);
    }
  }

  // Split apart the cart link on the -.
  $actions = explode('-', urldecode($arg1));
  $rebuild_cart = FALSE;

  foreach ($actions as $action) {
    switch (drupal_substr($action, 0, 1)) {
      // Set the ID of the cart link.
      case 'i':
      case 'I':
        $id = drupal_substr($action, 1);
        break;

      // Add a product to the cart.
      case 'p':
      case 'P':
        // Set the default product variables.
        $p = array('nid' => 0, 'qty' => 1, 'data' => array());
        $msg = TRUE;

        // Parse the product action to adjust the product variables.
        $parts = explode('_', $action);
        foreach ($parts as $part) {
          switch (drupal_substr($part, 0, 1)) {
            // Set the product node ID: p23
            case 'p':
            case 'P':
              $p['nid'] = intval(drupal_substr($part, 1));
              break;
            // Set the quantity to add to cart: q2
            case 'q':
            case 'Q':
              $p['qty'] = intval(drupal_substr($part, 1));
              break;
            // Set an attribute/option for the product: a3o6
            case 'a':
            case 'A':
              $attribute = intval(drupal_substr($part, 1, stripos($part, 'o') - 1));
              $option = (string) drupal_substr($part, stripos($part, 'o') + 1);
              // Multiple options for this attribute implies checkbox
              // attribute, which we must store as an array. However, all other
              // types can be stored as arrays as well.
              $p['attributes'][$attribute][$option] = $option;
              break;
            // Suppress the add to cart message: m0
            case 'm':
            case 'M':
              $msg = intval(drupal_substr($part, 1)) == 1 ? TRUE : FALSE;
              break;
          }
        }

        // Add the item to the cart, suppressing the default redirect.
        if ($p['nid'] > 0 && $p['qty'] > 0) {
          // If it's a product kit, we need black magic to make everything work
          // right. In other words, we have to simulate FAPI's form values.
          $node = node_load($p['nid']);
          if (is_array($node->products)) {
            foreach ($node->products as $nid => $product) {
              $p['data']['products'][$nid] = array(
                'nid' => $nid,
                'qty' => $product->qty,
              );
            }
          }
          uc_cart_add_item($p['nid'], $p['qty'], $p['data'] + module_invoke_all('uc_add_to_cart_data', $p), NULL, $msg, FALSE, FALSE);
          $rebuild_cart = TRUE;
        }
        break;

      // Empty the shopping cart.
      case 'e':
      case 'E':
        if (variable_get('uc_cart_links_empty', TRUE)) {
          uc_cart_empty(uc_cart_get_id());
        }
        break;

      // Display a pre-configured message.
      case 'm':
      case 'M':
        // Load the messages if they haven't been loaded yet.
        if (empty($messages)) {
          $data = explode("\n", variable_get('uc_cart_links_messages', ''));
          foreach ($data as $message) {
            $mdata = explode('|', $message);
            $messages[$mdata[0]] = $mdata[1];
          }
        }

        // Parse the message key and display it if it exists.
        $mkey = intval(drupal_substr($action, 1));
        if (!empty($messages[$mkey])) {
          drupal_set_message($messages[$mkey]);
        }
        break;
    }

    // Rebuild the cart cache if necessary.
    if ($rebuild_cart) {
      uc_cart_get_contents(NULL, 'rebuild');
    }
  }

  if (variable_get('uc_cart_links_track', TRUE)) {
    db_merge('uc_cart_link_clicks')
      ->key(array('cart_link_id' => $id))
      ->fields(array(
        'clicks' => 1,
        'last_click' => REQUEST_TIME,
      ))
      ->expression('clicks', 'clicks + :i', array(':i' => 1))
      ->execute();
  }

  $_SESSION['uc_cart_last_url'] = uc_referer_uri();

  drupal_goto();
}
