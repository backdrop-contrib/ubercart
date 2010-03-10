<?php
// $Id$

/**
 * @file
 * Hooks provided by the Order module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Used to define line items that are attached to orders.
 *
 * A line item is a representation of charges, fees, and totals for an order.
 * Default line items include the subtotal and total line items, the tax line
 * item, and the shipping line item. There is also a generic line item that store
 * admins can use to add extra fees and discounts to manually created orders.
 * Module developers will use this hook to define new types of line items for
 * their stores. An example use would be for a module that allows customers to
 * use coupons and wants to represent an entered coupon as a line item.
 *
 * Once a line item has been defined in hook_line_item, Übercart will begin
 * interacting with it in various parts of the code. One of the primary ways this
 * is done is through the callback function you specify for the line item.
 *
 * @return
 *   Your hook should return an array of associative arrays. Each item in the
 *   array represents a single line item and should use the following keys:
 *   - "id"
 *     - type: string
 *     - value: The internal ID of the line item.
 *   - "title"
 *     - type: string
 *     - value: The title of the line item shown to the user in various interfaces.
 *         Use t().
 *   - "callback"
 *     - type: string
 *     - value: Name of the line item's callback function, called for various
 *         operations.
 *   - "weight"
 *     - type: integer
 *     - value: Display order of the line item in lists; "lighter" items are
 *         displayed first.
 *   - "stored"
 *     - type: boolean
 *     - value: Whether or not the line item will be stored in the database.
 *         Should be TRUE for any line item that is modifiable from the order
 *         edit screen.
 *   - "add_list"
 *     - type: boolean
 *     - value: Whether or not a line item should be included in the "Add a Line
 *         Item" select box on the order edit screen.
 *   - "calculated"
 *     - type: boolean
 *     - value: Whether or not the value of this line item should be added to the
 *         order total. (Ex: would be TRUE for a shipping charge line item but
 *         FALSE for the subtotal line item since the product prices are already
 *         taken into account.)
 *   - "display_only"
 *     - type: boolean
 *     - value: Whether or not this line item is simply a display of information
 *         but not calculated anywhere. (Ex: the total line item uses display to
 *         simply show the total of the order at the bottom of the list of line
 *         items.)
 */
function hook_uc_line_item() {
  $items[] = array(
    'id' => 'generic',
    'title' => t('Empty Line'),
    'weight' => 2,
    'default' => FALSE,
    'stored' => TRUE,
    'add_list' => TRUE,
    'calculated' => TRUE,
    'callback' => 'uc_line_item_generic',
  );

  return $items;
}

/**
 * Alter a line item on an order when the order is loaded.
 *
 * @param &$item
 *   The line item array.
 * @param $order
 *   The order object containing the line item.
 */
function hook_uc_line_item_alter(&$item, $order) {
  $account = user_load($order->uid);
  ca_pull_trigger('calculate_line_item_discounts', $item, $account);
}

/**
 * Alter the line item definitions declared in hook_line_item().
 *
 * @param &$items
 *   The combined return value of hook_line_item().
 */
function hook_uc_line_item_data_alter(&$items) {
  foreach ($items as &$item) {
    // Tax amounts are added in to other line items, so the actual tax line
    // items should not be added to the order total.
    if ($item['id'] == 'tax') {
      $item['calculated'] = FALSE;
    }
    // Taxes are included already, so the subtotal without taxes doesn't
    // make sense.
    elseif ($item['id'] == 'tax_subtotal') {
      $item['callback'] = NULL;
    }
  }
}


/**
 * Perform actions on orders.
 *
 * An order in Übercart represents a single transaction. Orders are created
 * during the checkout process where they sit in the database with a status of In
 * Checkout. When a customer completes checkout, the order's status gets updated
 * to show that the sale has gone through. Once an order is created, and even
 * during its creation, it may be acted on by any module to connect extra
 * information to an order. Every time an action occurs to an order, hook_order()
 * gets invoked to let your modules know what's happening and make stuff happen.
 *
 * @param $op
 *   The action being performed.
 * @param &$arg1
 *   This is the order object or a reference to it as noted below.
 * @param $arg2
 *   This is variable and is based on the value of $op:
 *   - new: Called when an order is created. $arg1 is a reference to the new
 *       order object, so modules may add to or modify the order at creation.
 *   - save: When an order object is being saved, the hook gets invoked with this
 *       op to let other modules do any necessary saving. $arg1 is a reference to
 *       the order object.
 *   - load: Called when an order is loaded after the order and product data has
 *       been loaded from the database. Passes $arg1 as the reference to the
 *       order object, so modules may add to or modify the order object when it's
 *       loaded.
 *   - submit: When a sale is being completed and the customer has clicked the
 *       Submit order button from the checkout screen, the hook is invoked with
 *       this op. This gives modules a chance to determine whether or not the
 *       order should be allowed. An example use of this is the credit module
 *       attempting to process payments when an order is submitted and returning
 *       a failure message if the payment failed.
 *
 *       To prevent an order from passing through, you must return an array
 *       resembling the following one with the failure message:
 *       @code
 *         return array(array('pass' => FALSE, 'message' => t('We were unable to process your credit card.')));
 *       @endcode
 *   - can_update: Called before an order's status is changed to make sure the
 *       order can be updated. $arg1 is the order object with the old order
 *       status ID ($arg1->order_status), and $arg2 is simply the new order
 *       status ID. Return FALSE to stop the update for some reason.
 *   - update: Called when an order's status is changed. $arg1 is the order
 *       object with the old order status ID ($arg1->order_status), and $arg2 is
 *       the new order status ID.
 *   - can_delete: Called before an order is deleted to verify that the order may
 *       be deleted. Returning FALSE will prevent a delete from happening. (For
 *       example, the payment module returns FALSE by default when an order has
 *       already received payments.)
 *   - delete: Called when an order is deleted and before the rest of the order
 *       information is removed from the database. Passes $arg1 as the order
 *       object to let your module clean up it's tables.
 *   - total: Called when the total for an order is being calculated after the
 *       total of the products has been added. Passes $arg1 as the order object.
 *       Expects in return a value (positive or negative) by which to modify the
 *       order total.
 */
function hook_uc_order($op, &$arg1, $arg2) {
  switch ($op) {
    case 'save':
      // Do something to save payment info!
      break;
  }
}

/**
 * Add links to local tasks for orders on the admin's list of orders.
 *
 * @param $order
 *   An order object.
 * @return
 *   An array of specialized link arrays. Each link has the following keys:
 *   - "name": The title of page being linked.
 *   - "url": The link path. Do not use url(), but do use the $order's order_id.
 *   - "icon": HTML of an image.
 *   - "title": Title attribute text (mouseover tool-tip).
 */
function hook_uc_order_actions($order) {
  $actions = array();
  $module_path = base_path() . drupal_get_path('module', 'uc_shipping');
  if (user_access('fulfill orders')) {
    $result = db_query("SELECT nid FROM {uc_order_products} WHERE order_id = %d AND data LIKE '%%s:9:\"shippable\";s:1:\"1\";%%'", $order->order_id);
    if (db_num_rows($result)) {
      $title = t('Package order !order_id products.', array('!order_id' => $order->order_id));
      $actions[] = array(
        'name' => t('Package'),
        'url' => 'admin/store/orders/'. $order->order_id .'/packages',
        'icon' => '<img src="'. $module_path .'/images/package.gif" alt="'. $title .'" />',
        'title' => $title,
      );
      $result = db_query("SELECT package_id FROM {uc_packages} WHERE order_id = %d", $order->order_id);
      if (db_num_rows($result)) {
        $title = t('Ship order !order_id packages.', array('!order_id' => $order->order_id));
        $actions[] = array(
          'name' => t('Ship'),
          'url' => 'admin/store/orders/'. $order->order_id .'/shipments',
          'icon' => '<img src="'. $module_path .'/images/ship.gif" alt="'. $title .'" />',
          'title' => $title,
        );
      }
    }
  }
  return $actions;
}

/**
 * Register callbacks for an order pane.
 *
 * This hook is used to add panes to the order viewing and administration screens.
 * The default panes include areas to display and edit addresses, products,
 * comments, etc. Developers should use this hook when they need to display or
 * modify any custom data pertaining to an order. For example, a store that uses
 * a custom checkout pane to find out a customer's desired delivery date would
 * then create a corresponding order pane to show the data on the order screens.
 *
 * hook_order_pane() works by defining new order panes and providing a little bit
 * of information about them. View the return value section below for information
 * about what parts of an order pane are defined by the hook.
 *
 * The real meat of an order pane is its callback function (which is specified in
 * the hook). The callback function handles what gets displayed on which screen
 * and what data can be manipulated. That is all somewhat out of the scope of
 * this API page, so you'll have to click here to read more about what a callback
 * function should contain.
*/
function hook_uc_order_pane() {
  $panes[] = array(
    'id' => 'payment',
    'callback' => 'uc_order_pane_payment',
    'title' => t('Payment'),
    'desc' => t('Specify and collect payment for an order.'),
    'class' => 'pos-left',
    'weight' => 4,
    'show' => array('view', 'edit', 'customer'),
  );
  return $panes;
}

/**
 * Allows modules to alter ordered products when they're loaded with an order.
 *
 * @param &$product
 *   The product object as found in the $order object.
 * @param $order
 *   The order object to which the product belongs.
 * @return
 *   Nothing should be returned. Hook implementations should receive the
 *     $product object by reference and alter it directly.
 */
function hook_uc_order_product_alter(&$product, $order) {
  drupal_set_message('hook_order_product_alter(&$product, $order):');
  drupal_set_message('&$product: <pre>'. print_r($product, TRUE) .'</pre>');
  drupal_set_message('$order: <pre>'. print_r($order, TRUE) .'</pre>');
}

/**
 * Register static order states.
 *
 * Order states are module-defined categories for order statuses. Each state
 * will have a default status that is used when modules need to move orders to
 * new state, but don't know which status to use.
 *
 * @return
 *   An array of order state definitions. Each definition is an array with the
 *   following keys:
 *   - id: The machine-readable name of the order state.
 *   - title: The human-readable, translated name.
 *   - weight: The list position of the state.
 *   - scope: Either "specific" or "general".
 */
function hook_uc_order_state() {
  $states[] = array(
    'id' => 'canceled',
    'title' => t('Canceled'),
    'weight' => -20,
    'scope' => 'specific',
  );
  $states[] = array(
    'id' => 'in_checkout',
    'title' => t('In checkout'),
    'weight' => -10,
    'scope' => 'specific',
  );
  $states[] = array(
    'id' => 'post_checkout',
    'title' => t('Post checkout'),
    'weight' => 0,
    'scope' => 'general',
  );
  $states[] = array(
    'id' => 'completed',
    'title' => t('Completed'),
    'weight' => 20,
    'scope' => 'general',
  );

  return $states;
}

/**
 * @} End of "addtogroup hooks".
 */

