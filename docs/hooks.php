<?php
// $Id$

/**
 * @file
 * These are the hooks that are invoked by the Übercart core.
 *
 * Core hooks are typically called in all modules at once using
 * module_invoke_all().
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Do extra processing when an item is added to the shopping cart.
 *
 * Some modules need to be able to hook into the process of adding items to a
 * cart. For example, an inventory system may need to check stock levels and
 * prevent an out of stock item from being added to a customer's cart. This hook
 * lets developers squeeze right in at the end of the process after the product
 * information is all loaded and the product is about to be added to the cart.
 * In the event that a product should not be added to the cart, you simply have
 * to return a failure message described below. This hook may also be used simply
 * to perform some routine action when products are added to the cart.
 *
 * @param $nid
 *   The node ID of the product
 * @param $qty
 *   The quantity being added
 * @param $data
 *   The data array, including attributes and model number adjustments
 * @return
 *   The function can use this data to whatever purpose to see if the item can
 *   be added to the cart or not. The function should return an array containing
 *   the result array. (This is due to the nature of Drupal's module_invoke_all()
 *   function. You must return an array within an array or other module data will
 *   end up getting ignored.) At this moment, there are only three keys:
 *   - "success": TRUE or FALSE for whether the specified quantity of the item
 *       may be added to the cart or not; defaults to TRUE.
 *   - "message": the fail message to display in the event of a failure; if
 *       omitted, Ubercart will display a default fail message.
 *   - "silent": return TRUE to suppress the display of any messages; useful
 *       when a module simply needs to do some other processing during an add to
 *       cart or fail silently.
 */
function hook_add_to_cart($nid, $qty, $data) {
  if ($qty > 1) {
    $result[] = array(
      'success' => FALSE,
      'message' => t('Sorry, you can only add one of those at a time.'),
    );
  }
  return $result;
}

/**
 * Add extra information to a cart item's "data" array.
 *
 * This is effectively the submit handler of any alterations to the Add to Cart
 * form. It provides a standard way to store the extra information so that it
 * can be used by hook_add_to_cart().
 *
 * @param $form_values
 *   The values submitted to the Add to Cart form.
 * @return
 *   An array of data to be merged into the item added to the cart.
 */
function hook_add_to_cart_data($form_values) {
  $node = node_load($form_values['nid']);
  return array('module' => 'uc_product', 'shippable' => $node->shippable);
}

/**
 * Control the display of an item in the cart.
 *
 * Product type modules allow the creation of nodes that can be added to the
 * cart. The cart determines how they are displayed through this hook. This is
 * especially important for product kits, because it may be displayed as a single
 * unit in the cart even though it is represented as several items.
 *
 * @param $item
 *   The item in the cart to display.
 * @return
 *   A form array containing the following elements:
 *   - "nid"
 *     - #type: value
 *     - #value: The node id of the $item.
 *   - "module"
 *     - #type: value
 *     - #value: The module implementing this hook and the node represented by
 *       $item.
 *   - "remove"
 *     - #type: checkbox
 *     - #value: If selected, removes the $item from the cart.
 *   - "options"
 *     - #type: markup
 *     - #value: Themed markup (usually an unordered list) displaying extra information.
 *   - "title"
 *     - #type: markup
 *     - #value: The displayed title of the $item.
 *   - "#total"
 *     - "type": float
 *     - "value": Numeric price of $item. Notice the '#' signifying that this is
 *       not a form element but just a value stored in the form array.
 *   - "data"
 *     - #type: hidden
 *     - #value: The serialized $item->data.
 *   - "qty"
 *     - #type: textfield
 *     - #value: The quantity of $item in the cart. When "Update cart" is clicked,
 *         the customer's input is saved to the cart.
 */
function hook_cart_display($item) {
  $node = node_load($item->nid);
  $element = array();
  $element['nid'] = array('#type' => 'value', '#value' => $node->nid);
  $element['module'] = array('#type' => 'value', '#value' => 'uc_product');
  $element['remove'] = array('#type' => 'checkbox');
  $op_names = '';
  if (module_exists('uc_attribute')){
    $op_names = "<ul class=\"cart-options\">\n";
    foreach ($item->options as $option){
      $op_names .= '<li>'. $option['attribute'] .': '. $option['name'] ."</li>\n";
    }
    $op_names .= "</ul>\n";
  }
  $element['options'] = array('#value' => $op_names);
  $element['title'] = array(
    '#value' => l($node->title, 'node/'. $node->nid),
  );
  $element['#total'] = $item->price * $item->qty;
  $element['data'] = array('#type' => 'hidden', '#value' => serialize($item->data));
  $element['qty'] = array(
    '#type' => 'textfield',
    '#default_value' => $item->qty,
    '#size' => 3,
    '#maxlength' => 3
  );
  return $element;
}

/**
 * Add extra data about an item in the cart.
 *
 * Products that are added to a customer's cart are referred as items until the
 * sale is completed. Just think of a grocery store having a bunch of products
 * on the shelves but putting a sign over the express lane saying "15 Items or
 * Less." hook_cart_item() is in charge of acting on items at various times like
 * when they are being added to a cart, saved, loaded, and checked out.
 *
 * Here's the rationale for this hook: Products may change on a live site during
 * a price increase or change to attribute adjustments. If a user has previously
 * added an item to their cart, when they go to checkout or view their cart
 * screen we want the latest pricing and model numbers to show. So, the essential
 * product information is stored in the cart, but when the items in a cart are
 * loaded, modules are given a chance to adjust the data against the latest settings.
 *
 * @param $op
 *   The action that is occurring. Possible values:
 *   - "load" - Passed for each item when a cart is being loaded in the function
 *       uc_cart_get_contents(). This gives modules the chance to tweak information
 *       for items when the cart is being loaded prior to being view or added to
 *       an order. No return value is expected.
 *   - "can_ship" - Passed when a cart is being scanned for items that are not
 *       shippable items. Übercart will bypass cart and checkout operations
 *       specifically related to tangible products if nothing in the cart is
 *       shippable. hook_cart_item functions that check for this op are expected
 *       to return TRUE or FALSE based on whether a product is shippable or not.
 * @return
 *   No return value for load.
 *   TRUE or FALSE for can_ship.
 */
function hook_cart_item($op, &$item) {
  switch ($op) {
    case 'load':
      $term = array_shift(taxonomy_node_get_terms_by_vocabulary($item->nid, variable_get('uc_manufacturer_vid', 0)));
      $arg1->manufacturer = $term->name;
      break;
  }
}

/**
 * Register callbacks for a cart pane.
 *
 * The default cart view page displays a table of the cart contents and a few
 * simple form features to manage the cart contents. For a module to add
 * information to this page, it must use hook_cart_pane to define extra panes
 * that may be ordered to appear above or below the default information.
 *
 * @param $items
 *   The current contents of the shopping cart.
 * @return
 *   The function is expected to return an array of pane arrays with the following
 *   keys:
 *   - "id"
 *     - type: string
 *     - value: The internal ID of the pane, using a-z, 0-9, and - or _.
 *   - "title"
 *     - type: string
 *     - value: The name of the cart pane displayed to the user.  Use t().
 *   - "enabled"
 *     - type: boolean
 *     - value: Whether the pane is enabled by default or not. (Defaults to TRUE.)
 *   - "weight"
 *     - type: integer
 *     - value: The weight of the pane to determine its display order. (Defaults
 *         to 0.)
 *   - "body"
 *     - type: string
 *     - value: The body of the pane when rendered on the cart view screen.
 *
 * The body gets printed to the screen if it is on the cart view page.  For the
 * settings page, the body field is ignored.  You may want your function to check
 * for a NULL argument before processing any queries or foreach() loops.
 */
function hook_cart_pane($items) {
  $panes[] = array(
    'id' => 'cart_form',
    'title' => t('Default cart form'),
    'enabled' => TRUE,
    'weight' => 0,
    'body' => !is_null($items) ? drupal_get_form('uc_cart_view_form', $items) : '',
  );

  return $panes;
}

/**
 * Register callbacks for a checkout pane.
 *
 * The checkout screen for Ubercart is a compilation of enabled checkout panes.
 * A checkout pane can be used to display order information, collect data from
 * the customer, or interact with other panes. Panes are defined in enabled modules
 * with hook_checkout_pane() and displayed and processed through specified callback
 * functions. Some of the settings for each pane are configurable from the checkout
 * settings page with defaults being specified in the hooks.
 *
 * The default panes are defined in uc_cart.module in the function
 * uc_cart_checkout_pane(). These include panes to display the contents of the
 * shopping cart and to collect essential site user information, a shipping address,
 * a payment address, and order comments. Other included modules offer panes for
 * shipping and payment purposes as well.
 *
 * @return
 *   An array of checkout pane arrays using the following keys:
 *   - "id"
 *     - type: string
 *     - value: The internal ID of the checkout pane, using a-z, 0-9, and - or _.
 *   - "title"
 *     - type: string
 *     - value:The name of the pane as it appears on the checkout form.
 *   - "desc"
 *     - type: string
 *     - value: A short description of the pane for the admin pages.
 *   - "callback"
 *     - type: string
 *     - value: The name of the callback function for this pane.  View
 *         @link http://www.ubercart.org/docs/developer/245/checkout this page @endlink
 *         for more documentation and examples of checkout pane callbacks.
 *   - "weight"
 *     - type: integer
 *     - value: Default weight of the pane, defining its order on the checkout form.
 *   - "enabled"
 *     - type: boolean
 *     - value: Optional. Whether or not the pane is enabled by default. Defaults
 *         to TRUE.
 *   - "process"
 *     - type: boolean
 *     - value: Optional. Whether or not this pane needs to be processed when the
 *         checkout form is submitted. Defaults to TRUE.
 *   - "collapsible"
 *     - type: boolean
 *     - value: Optional. Whether or not this pane is displayed as a collapsible
 *         fieldset. Defaults to TRUE.
 */
function hook_checkout_pane() {
  $panes[] = array(
    'id' => 'cart',
    'callback' => 'uc_checkout_pane_cart',
    'title' => t('Cart Contents'),
    'desc' => t("Display the contents of a customer's shopping cart."),
    'weight' => 1,
    'process' => FALSE,
    'collapsible' => FALSE,
  );
  return $panes;
}

/**
 * Give clearance to a user to download a file.
 *
 * By default the uc_file module can implement 3 restrictions on downloads: by
 * number of IP addresses downloaded from, by number of downloads, and by a set
 * expiration date. Developers wishing to add further restrictions can do so by
 * implementing this hook. After the 3 aforementioned restrictions are checked,
 * the uc_file module will check for implementations of this hook.
 *
 * @param $user
 *   The drupal user object that has requested the download
 * @param $file_download
 *   The file download object as defined as a row from the uc_file_users table
 *   that grants the user the download
 * @return
 *   TRUE or FALSE depending on whether the user is to be permitted download of
 *   the requested files. When a implementation returns FALSE it should set an
 *   error message in Drupal using drupal_set_message() to inform customers of
 *   what is going on.
 */
function hook_download_authorize($user, $file_download) {
  if (!$user->status) {
    drupal_set_message(t("This account has been banned and can't download files anymore. "),'error');
    return FALSE;
  }
  else {
    return TRUE;
  }
}

/**
 * Perform actions on file products.
 *
 * The uc_file module comes with a file manager (found at Administer » Store
 * administration » Products » View file downloads) that provides some basic
 * functionality: deletion of multiple files and directories, and upload of single
 * files (those looking to upload multiple files should just directly upload them
 * to their file download directory then visit the file manager which automatically
 * updates new files found in its directory). Developers that need to create more
 * advanced actions with this file manager can do so by using this hook.
 *
 * @param $op
 *   The operation being taken by the hook, possible ops defined below.
 *   - 'info': Called before the uc_file module builds its list of possible file
 *       actions. This op is used to define new actions that will be placed in
 *       the file action select box.
 *   - 'insert': Called after uc_file discovers a new file in the file download
 *       directory.
 *   - 'form': When any defined file action is selected and submitted to the form
 *       this function is called to render the next form. Because this is called
 *       whenever a module-defined file action is selected, the variable
 *       $args['action'] can be used to define a new form or append to an existing
 *       form.
 *   - 'upload': After a file has been uploaded, via the file manager's built in
 *       file upload function, and moved to the file download directory this op
 *       can perform any remaining operations it needs to perform on the file
 *       before its placed into the uc_files table.
 *   - 'upload_validate': This op is called to validate the uploaded file that
 *       was uploaded via the file manager's built in file upload function. At
 *       this point, the file has been uploaded to PHP's temporary directory.
 *       Files passing this upload validate function will be moved into the file
 *       downloads directory.
 *   - 'validate': This op is called to validate the file action form.
 *   - 'submit': This op is called to submit the file action form.
 * @param $args
 *   A keyed array of values that varies depending on the op being performed,
 *   possible values defined below.
 *   - 'info': None
 *   - 'insert':
 *     - 'file_object': The file object of the newly discovered file
 *   - 'form':
 *     - 'action': The file action being performed as defined by the key in the
 *         array sent by hook_file_action($op = 'info')
 *     - 'file_ids' - The file ids (as defined in the uc_files table) of the
 *          selected files to perform the action on
 *   - 'upload':
 *     - 'file_object': The file object of the file moved into file downloads
 *         directory
 *     - 'form_id': The form_id variable of the form_submit function
 *     - 'form_values': The form_values variable of the form_submit function
 *   - 'upload_validate':
 *     - 'file_object': The file object of the file that has been uploaded into
 *         PHP's temporary upload directory
 *     - 'form_id': The form_id variable of the form_validate function
 *     - 'form_values': The form_values variable of the form_validate function
 *   - 'validate':
 *     - 'form_id': The form_id variable of the form_validate function
 *     - 'form_values': The form_values variable of the form_validate function
 *   - 'submit':
 *     - 'form_id': The form_id variable of the form_submit function
 *     - 'form_values': The form_values variable of the form_submit function
 * @return
 *   The return value of hook depends on the op being performed, possible return
 *   values defined below.
 *   - 'info': The associative array of possible actions to perform. The keys are
 *       unique strings that defines the actions to perform. The values are the
 *       text to be displayed in the file action select box.
 *   - 'insert': None
 *   - 'form': This op should return an array of drupal form elements as defined
 *       by the drupal form API.
 *   - 'upload': None
 *   - 'upload_validate': None
 *   - 'validate': None
 *   - 'submit': None
 */
function hook_file_action($op, $args) {
  switch ($op) {
    case 'info':
      return array('uc_image_watermark_add_mark' => 'Add Watermark');
    case 'insert':
      //automatically adds watermarks to any new files that are uploaded to the file download directory
      _add_watermark($args['file_object']->filepath);
    break;
    case 'form':
      if ($args['action'] == 'uc_image_watermark_add_mark') {
        $form['watermark_text'] = array(
          '#type' => 'textfield',
          '#title' => t('Watermark Text'),
        );
        $form['submit_watermark'] = array(
          '#type' => 'submit',
          '#value' => t('Add Watermark'),
        );
      }
    return $form;
    case 'upload':
      _add_watermark($args['file_object']->filepath);
      break;
    case 'upload_validate':
      //Given a file path, function checks if file is valid JPEG
      if(!_check_image($args['file_object']->filepath)) {
        form_set_error('upload',t('Uploaded file is not a valid JPEG'));
      }
    break;
    case 'validate':
      if ($args['form_values']['action'] == 'uc_image_watermark_add_mark') {
        if (empty($args['form_values']['watermark_text'])) {
          form_set_error('watermar_text',t('Must fill in text'));
        }
      }
    break;
    case 'submit':
      if ($args['form_values']['action'] == 'uc_image_watermark_add_mark') {
        foreach ($args['form_values']['file_ids'] as $file_id) {
          $filename = db_result(db_query("SELECT filename FROM {uc_files} WHERE fid = %d",$file_id));
          //Function adds watermark to image
          _add_watermark($filename);
        }
      }
    break;
  }
}

/**
 * Make changes to a file before it is downloaded by the customer.
 *
 * Stores, either for customization, copy protection or other reasons, might want
 * to send customized downloads to customers. This hook will allow this to happen.
 * Before a file is opened to be transfered to a customer, this hook will be called
 * to make any altercations to the file that will be used to transfer the download
 * to the customer. This, in effect, will allow a developer to create a new,
 * personalized, file that will get transfered to a customer.
 *
 * @param $file_user
 *   The file_user object (i.e. an object containing a row from the uc_file_users
 *   table) that corresponds with the user download being accessed.
 * @param $ip
 *   The IP address from which the customer is downloading the file
 * @param $fid
 *   The file id of the file being transfered
 * @param $file
 *   The file path of the file to be transfered
 * @return
 *   The path of the new file to transfer to customer.
 */
function hook_file_transfer_alter($file_user, $ip, $fid, $file) {
  $file_data = file_get_contents($file)." [insert personalized data]"; //for large files this might be too memory intensive
  $new_file = tempnam(file_directory_temp(),'tmp');
  file_put_contents($new_file,$file_data);
  return $new_file;
}

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
function hook_line_item() {
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
function hook_order($op, &$arg1, $arg2) {
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
function hook_order_actions($order) {
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
function hook_order_pane() {
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
 * Register payment gateway callbacks.
 *
 * @see @link http://www.ubercart.org/docs/api/hook_payment_gateway @endlink
 *
 * @return
 *   Returns an array of payment gateways, which are arrays with the following keys:
 *   - "id"
 *     - type: string
 *     - value: The internal ID of the payment gateway, using a-z, 0-9, and - or
 *         _.
 *   - "title"
 *     - type: string
 *     - value: The name of the payment gateway displayed to the user. Use t().
 *   - "description"
 *     - type: string
 *     - value: A short description of the payment gateway.
 *   - "settings"
 *     - type: string
 *     - value: The name of a function that returns an array of settings form
 *         elements for the gateway.
 */
function hook_payment_gateway() {
  $gateways[] = array(
    'id' => 'test_gateway',
    'title' => t('Test Gateway'),
    'description' => t('Process credit card payments through the Test Gateway.'),
    'credit' => 'test_gateway_charge',
  );
  return $gateways;
}

/**
 * Register callbacks for payment methods.
 *
 * Payment methods are different ways to collect payment. By default, Übercart
 * comes with support for check, credit card, and generic payments. Payment
 * methods show up at checkout or on the order administration screens, and they
 * collect different sorts of information from the user that is used to process
 * or track the payment.
 *
 * @return
 *   An array of payment methods.
 */
function hook_payment_method() {
  $methods[] = array(
    'id' => 'check',
    'name' => t('Check'),
    'title' => t('Check or Money Order'),
    'desc' => t('Pay by mailing a check or money order.'),
    'callback' => 'uc_payment_method_check',
    'weight' => 1,
    'checkout' => TRUE,
  );
  return $methods;
}

/**
 * Perform actions on product classes.
 *
 * @param $type
 *   The node type of the product class.
 * @param $op
 *   The action being performed on the product class:
 *   - "insert": A new node type is created, or an existing node type is being
 *       converted into a product type.
 *   - "update": A product class has been updated.
 *   - "delete": A product class has been deleted. Modules that have attached
 *       additional information to the node type because it is a product type
 *       should delete this information.
 */
function hook_product_class($type, $op) {
  switch ($op) {
    case 'delete':
      db_query("DELETE FROM {uc_class_attributes} WHERE pcid = '%s'", $type);
      db_query("DELETE FROM {uc_class_attribute_options} WHERE pcid = '%s'", $type);
    break;
  }
}

/**
 * List node types which should be considered products.
 *
 * Trusts the duck philosophy of object identification: if it walks like a duck,
 * quacks like a duck, and has feathers like a duck, it's probably a duck.
 * Products are nodes with prices, SKUs, and everything else Übercart expects
 * them to have.
 *
 * @return
 *   Array of node type ids.
 */
function hook_product_types() {
  return array('product_kit');
}

/**
 * Handle additional data for shipments.
 *
 * @param $op
 *   The action being taken on the shipment. One of the following values:
 *   - "load": The shipment and its packages are loaded from the database.
 *   - "save": Changes to the shipment have been written.
 *   - "delete": The shipment has been deleted and the packages are available
 *     for reshipment.
 * @param &$shipment
 *   The shipment object.
 * @return
 *   Only given when $op is "load". An array of extra data to be added to the
 *   shipment object.
 */
function hook_shipment($op, &$shipment) {
  switch ($op) {
    case 'save':
      $google_order_number = uc_google_checkout_get_google_number($shipment->order_id);
      if ($google_order_number && $shipment->is_new) {
        $xml_data = '';
        foreach ($shipment->packages as $package) {
          if ($package->tracking_number) {
            $tracking_number = $package->tracking_number;
          }
          else if ($shipment->tracking_number) {
            $tracking_number = $shipment->tracking_number;
          }
          if ($tracking_number) {
            foreach ($package->products as $product) {
              $xml_data .= '<item-shipping-information>';
              $xml_data .= '<item-id>';
              $xml_data .= '<merchant-item-id>'. check_plain($product->nid .'|'. $product->model) .'</merchant-item-id>';
              $xml_data .= '</item-id>';
              $xml_data .= '<tracking-data-list>';
              $xml_data .= '<tracking-data>';
              $xml_data .= '<carrier>'. check_plain($shipment->carrier) .'</carrier>';
              $xml_data .= '<tracking-number>'. check_plain($tracking_number) .'</tracking-number>';
              $xml_data .= '</tracking-data>';
              $xml_data .= '</tracking-data-list>';
              $xml_data .= '</item-shipping-information>';
            }
          }
        }
        if ($xml_data) {
          $request = '<?xml version="1.0" encoding="UTF-8"?>'. "\n";
          $request .= '<ship-items xmlns="http://checkout.google.com/schema/2" google-order-number="'. $google_order_number .'">';
          $request .= '<item-shipping-information-list>';
          $request .= $xml_data;
          $request .= '</item-shipping-information-list>';
          $request .= '<send-email>true</send-email>';
          $request .= '</ship-items>';
          $response = uc_google_checkout_send_request('request', $request);
        }
      }
    break;
    case 'delete':
      $google_order_number = uc_google_checkout_get_google_number($shipment->order_id);
      if ($google_order_number) {
        foreach ($shipment->packages as $package) {
          foreach ($package->products as $product) {
            $reset_ids[] = check_plain($product->nid .'|'. $product->model);
          }
        }
        $request = '<?xml version="1.0" encoding="UTF-8"?>' . "\n";
        $request .= '<reset-items-shipping-information xmlns="http://checkout.google.com/schema/2" google-order-number="'. $google_order_number .'">';
        $request .= '<item-ids>';
        foreach (array_unique($reset_ids) as $item_id) {
          $request .= '<item-id>';
          $request .= '<merchant-item-id>'. $item_id .'</merchant-item-id>';
          $request .= '</item-id>';
        }
        $request .= '</item-ids>';
        $request .= '<send-email>false</send-email>';
        $request .= '</reset-items-shipping-information>';
      }
      $response = uc_google_checkout_send_request('request', $request);
    break;
  }
}

/**
 * Define callbacks and service options for shipping methods.
 *
 * The shipping quote controller module, uc_quote, expects a very specific
 * structured array of methods from the implementations of this hook.
 *
 * The weights and enabled flags for shipping methods and types are set at the
 * Shipping Quote Settings page under Store Configuration. They keys of the
 * variables are the ids of the shipping methods. The "quote" and "ship" arrays of
 * the method are both optional.
 *
 * @return
 *   An array of shipping methods which have the following keys.
 *   - "type": The quote and shipping types are ids of the product shipping type
 *       that these methods apply to. type may also be 'order' which indicates
 *       that the quote applies to the entire order, regardless of the shipping
 *       types of its products. This is used by quote methods that are base on
 *       the location of the customer rather than their purchase.
 *   - "callback": The function that is called by uc_quote when a shipping quote
 *       is requested. Its arguments are the array of products and an array of
 *       order details (the shipping address). The return value is an array
 *       representing the rates quoted and errors returned (if any) for each
 *       option in the accessorials array.
 *   - "accessorials": This array represents the different options the customer
 *       may choose for their shipment. The callback function should generate a
 *       quote for each option in accessorials and return them via an array.
 *       drupal_to_js() is very useful for this.
 *       @code
 *         return array(
 *           '03' => array('rate' => 15.75, 'format' => uc_currency_format(15.75) 'option_label' => t('UPS Ground'),
 *                         'error' => 'Additional handling charge automatically applied.'),
 *           '14' => array('error' => 'Invalid package type.'),
 *           '59' => array('rate' => 26.03, 'format' => uc_currency_format(26.03), 'option_label' => t('UPS 2nd Day Air A.M.'))
 *         );
 *       @endcode
 *   - "pkg_types": The list of package types that the shipping method can handle.
 *       This should be an associative array that can be used as the #options of
 *       a select form element. It is recommended that a function be written to
 *       output this array so the method doesn't need to be found just for the
 *       package types.
 */
function hook_shipping_method() {
  $methods = array();

  $enabled = variable_get('uc_quote_enabled', array('ups' => true));
  $weight = variable_get('uc_quote_method_weight', array('ups' => 0));
  $methods['ups'] = array(
    'id' => 'ups',
    'title' => t('UPS'),
    'enabled' => $enabled['ups'],
    'weight' => $weight['ups'],
    'quote' => array(
      'type' => 'small package',
      'callback' => 'uc_ups_quote',
      'accessorials' => array(
        '03' => t('UPS Ground'),
        '11' => t('UPS Standard'),
        '01' => t('UPS Next Day Air'),
        '13' => t('UPS Next Day Air Saver'),
        '14' => t('UPS Next Day Early A.M.'),
        '02' => t('UPS 2nd Day Air'),
        '59' => t('UPS 2nd Day Air A.M.'),
        '12' => t('UPS 3-Day Select'),
      ),
    ),
    'ship' => array(
      'type' => 'small package',
      'callback' => 'uc_ups_fulfill_order',
      'pkg_types' => array(
        '01' => t('UPS Letter'),
        '02' => t('Customer Supplied Package'),
        '03' => t('Tube'),
        '04' => t('PAK'),
        '21' => t('UPS Express Box'),
        '24' => t('UPS 25KG Box'),
        '25' => t('UPS 10KG Box'),
        '30' => t('Pallet'),
      ),
    ),
  );

  return $methods;
}

/**
 * Define shipping types for shipping methods.
 *
 * This hook defines a shipping type that this module is designed to handle.
 * These types are specified by a machine- and human-readable name called 'id',
 * and 'title' respectively. Shipping types may be set for individual products,
 * manufacturers, and for the entire store catalog. Shipping modules should be
 * careful to use the same shipping type ids as other similar shipping modules
 * (i.e., FedEx and UPS both operate on "small package" shipments). Modules that
 * do not fulfill orders may not need to implement this hook.
 *
 * @return
 *   An array of shipping types keyed by a machine-readable name.
 */
function hook_shipping_type() {
  $weight = variable_get('uc_quote_type_weight', array('small_package' => 0));

  $types = array();
  $types['small_package'] = array(
    'id' => 'small_package',
    'title' => t('Small Packages'),
    'weight' => $weight['small_package'],
  );

  return $types;
}

/**
 * Add status messages to the "Store administration" page.
 *
 * This hook is used to add items to the store status table on the main store
 * administration screen. Each item gets a row in the table that consists of a
 * status icon, title, and description. These items should be used to give
 * special instructions, notifications, or indicators for components of the cart
 * enabled by the modules. At a glance, a store owner should be able to look here
 * and see if a critical component of your module is not functioning properly.
 *
 * For example, if the catalog module is installed and it cannot find the catalog
 * taxonomy vocabulary, it will show an error message here to alert the store
 * administrator.
 *
 * @return
 *   An array of tore status items which are arrays with the following keys:
 *   - "status": "ok", "warning", or "error" depending on the message.
 *   - "title" The title of the status message or module that defines it.
 *   - "desc": The description; can be any message, including links to pages and
 *       forms that deal with the issue being reported.
 */
function hook_store_status() {
  if ($key = uc_credit_encryption_key()) {
    $statuses[] = array(
      'status' => 'ok',
      'title' => t('Credit card encryption'),
      'desc' => t('Credit card data in the database is currently being encrypted.'),
    );
  }
  return $statuses;
}

/**
 * Convenience function to display large blocks of text in several places.
 *
 * There are many instances where Ubercart modules have configurable blocks of
 * text. These usually come with default messages, like e-mail templates for new
 * orders. Because of the way default values are normally set, you're then stuck
 * having to copy and paste a large chunk of text in at least two different
 * places in the module (when you're wanting to use the variable or to display
 * the settings form with the default value). To cut down code clutter, this hook
 * was introduced. It lets you put your messages in one place and use the
 * function uc_get_message() to retrieve the default value at any time (and from
 * any module).
 *
 * The function is very simple, expecting no arguments and returning a basic
 * associative array with keys being message IDs and their values being the
 * default message. When you call uc_get_message(), use the message ID you set
 * here to refer to the message you want.
 *
 * Note: When using t(), you must not pass it a concatenated string! So our
 * example has no line breaks in the message even though it is much wider than 80
 * characters. Using concatenation breaks translation.
 *
 * @return
 *   An array of messages.
 */
function hook_uc_message() {
  $messages['configurable_message_example'] = t('This block of text represents a configurable message such as a set of instructions or an e-mail template.  Using hook_uc_message to handle the default values for these is so easy even your grandma can do it!');

  return $messages;
}

/**
 * Handle requests to update a cart item.
 *
 * @param $nid
 *   Node id of the cart item.
 * @param $data
 *   Array of extra information about the item.
 * @param $qty
 *   The quantity of this item in the cart.
 * @param $cid
 *   The cart id. Defaults to NULL, which indicates that the current user's cart
 *   should be retrieved with uc_cart_get_id().
 */
function hook_update_cart_item($nid, $data = array(), $qty, $cid = NULL) {
  if (!$nid) return NULL;
  $cid = !(is_null($cid) || empty($cid)) ? $cid : uc_cart_get_id();
  if ($qty < 1) {
    uc_cart_remove_item($nid, $cid, $data);
  }
  else {
    db_query("UPDATE {uc_cart_products} SET qty = %d, changed = %d WHERE nid = %d AND cart_id = '%s' AND data = '%s'", $qty, time(), $nid, $cid, serialize($data));
    cache_clear_all();
  }

  // Rebuild the items hash
  uc_cart_get_contents(NULL, 'rebuild');
  if (!substr(request_uri(), 'cart', -4)) {
    drupal_set_message(t('Your item(s) have been updated.'));
  }
}

/**
 * @} End of "addtogroup hooks".
 */
