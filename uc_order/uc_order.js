// $Id$

/**
 * @file
 * Handles asynchronous requests for order editing forms.
 */

var customer_select = '';
var add_product_browser = '';
var order_save_holds = 0;

/**
 * Add the double click behavior to the order table at admin/store/orders.
 */
Drupal.behaviors.ucOrderClick = {
  attach: function(context, settings) {
    jQuery('.uc-orders-table tr.odd, .uc-orders-table tr.even:not(.ucOrderClick-processed)', context).addClass('ucOrderClick-processed').each(
      function() {
        jQuery(this).dblclick(
          function() {
            var url = settings.ucURL.adminOrders + this.id.substring(6);
            window.location = url;
          }
        );
      }
    );
  }
}

/**
 * Add the submit behavior to the order form
 */
Drupal.behaviors.ucOrderSubmit = {
  attach: function(context, settings) {
    jQuery('#uc-order-edit-form:not(.ucOrderSubmit-processed)', context).addClass('ucOrderSubmit-processed').submit(
      function() {
        jQuery('#products-selector').empty().removeClass();
        jQuery('#delivery_address_select').empty().removeClass();
        jQuery('#billing_address_select').empty().removeClass();
        jQuery('#customer-select').empty().removeClass();
      }
    );
  }
}

jQuery(document).ready(
  function() {
    if (order_save_holds == 0) {
      release_held_buttons();
    }

    jQuery('.uc-orders-table tr.odd, .uc-orders-table tr.even').each(
      function() {
        jQuery(this).dblclick(
          function() {
            var url = Drupal.settings.ucURL.adminOrders + this.id.substring(6);
            window.location = url;
          }
        );
      }
    );

    jQuery('#uc-order-edit-form').submit(
      function() {
        jQuery('#products-selector').empty().removeClass();
        jQuery('#delivery_address_select').empty().removeClass();
        jQuery('#billing_address_select').empty().removeClass();
        jQuery('#customer-select').empty().removeClass();
      }
    );
  }
);

/**
 * Copy the shipping data on the order edit screen to the corresponding billing
 * fields if they exist.
 */
function uc_order_copy_shipping_to_billing() {
  if (jQuery('#edit-delivery-zone').html() != jQuery('#edit-billing-zone').html()) {
    jQuery('#edit-billing-zone').empty().append(jQuery('#edit-delivery-zone').children().clone());
  }

  jQuery('#uc-order-edit-form input, select, textarea').each(
    function() {
      if (this.id.substring(0, 13) == 'edit-delivery') {
        jQuery('#edit-billing' + this.id.substring(13)).val(jQuery(this).val());
      }
    }
  );
}

/**
 * Load the address book div on the order edit screen.
 */
function load_address_select(uid, div, address_type) {
  var options = {
    'uid'  : uid,
    'type' : address_type,
    'func' : "apply_address('" + address_type + "', this.value);"
  };

  jQuery.post(Drupal.settings.ucURL.adminOrders + 'address_book', options,
    function (contents) {
      jQuery(div).empty().addClass('address-select-box').append(contents);
    }
  );
}

/**
 * Apply the selected address to the appropriate fields in the order edit form.
 */
function apply_address(type, address_str) {
  eval('var address = ' + address_str + ';');
  jQuery('#edit-' + type + '-first-name').val(address['first_name']);
  jQuery('#edit-' + type + '-last-name').val(address['last_name']);
  jQuery('#edit-' + type + '-phone').val(address['phone']);
  jQuery('#edit-' + type + '-company').val(address['company']);
  jQuery('#edit-' + type + '-street1').val(address['street1']);
  jQuery('#edit-' + type + '-street2').val(address['street2']);
  jQuery('#edit-' + type + '-city').val(address['city']);
  jQuery('#edit-' + type + '-postal-code').val(address['postal_code']);

  if (jQuery('#edit-' + type + '-country').val() != address['country']) {
    jQuery('#edit-' + type + '-country').val(address['country']);
    try {
      uc_update_zone_select('edit-' + type + '-country', address['zone']);
    }
    catch (err) {}
  }

  jQuery('#edit-' + type + '-zone').val(address['zone']);
}

/**
 * Close the address book div.
 */
function close_address_select(div) {
  jQuery(div).empty().removeClass('address-select-box');
  return false;
}

/**
 * Load the customer select div on the order edit screen.
 */
function load_customer_search() {
  if (customer_select == 'search' && jQuery('#customer-select #edit-back').val() == null) {
    return close_customer_select();
  }

  jQuery.post(Drupal.settings.ucURL.adminOrders + 'customer', {},
    function (contents) {
      jQuery('#customer-select').empty().addClass('customer-select-box').append(contents);
      jQuery('#customer-select #edit-first-name').val(jQuery('#edit-billing-first-name').val());
      jQuery('#customer-select #edit-last-name').val(jQuery('#edit-billing-last-name').val());
      customer_select = 'search';
    }
  );

  return false;
}

/**
 * Display the results of the customer search.
 */
function load_customer_search_results() {
  var first_name = jQuery('#customer-select #edit-first-name').val();
  var last_name = jQuery('#customer-select #edit-last-name').val();
  var email = jQuery('#customer-select #edit-email').val();

  if (first_name == '') {
    first_name = '0';
  }
  if (last_name == '') {
    last_name = '0';
  }
  if (email == '') {
    email = '0';
  }

  jQuery.post(Drupal.settings.ucURL.adminOrders + 'customer/search/' + encodeURIComponent(first_name) + '/' + encodeURIComponent(last_name) + '/' + encodeURIComponent(email),
    { },
    function (contents) {
      jQuery('#customer-select').empty().append(contents);
    }
  );
  return false;
}

/**
 * Set customer values from search selection.
 */
function select_customer_search() {
  var data = jQuery('#edit-cust-select').val();
  jQuery('#edit-uid').val(data.substr(0, data.indexOf(':')));
  jQuery('#edit-uid-text').val(data.substr(0, data.indexOf(':')));
  jQuery('#edit-primary-email').val(data.substr(data.indexOf(':') + 1));
  jQuery('#edit-primary-email-text').val(data.substr(data.indexOf(':') + 1));
  try {
    jQuery('#edit-submit-changes').get(0).click();
  }
  catch (err) {
  }
  return close_customer_select();
}

/**
 * Display the new customer form.
 */
function load_new_customer_form() {
  if (customer_select == 'new') {
    return close_customer_select();
  }

  jQuery.post(Drupal.settings.ucURL.adminOrders + 'customer/new', {},
    function (contents) {
      jQuery('#customer-select').empty().addClass('customer-select-box').append(contents);
      customer_select = 'new';
    }
  );
  return false;
}

/**
 * Validate the customer's email address.
 */
function check_new_customer_address() {
  var options = {
    'email' : jQuery('#customer-select #edit-email').val(),
    'sendmail' : jQuery('#customer-select #edit-sendmail').attr('checked')
  };
  jQuery.post(Drupal.settings.ucURL.adminOrders + 'customer/new/check/' + encodeURIComponent(options['email']), options,
    function (contents) {
      jQuery('#customer-select').empty().append(contents);
    }
  );
  return false;
}

/**
 * Load existing customer as new order's customer.
 */
function select_existing_customer(uid, email) {
  jQuery('#edit-uid').val(uid);
  jQuery('#edit-uid-text').val(uid);
  jQuery('#edit-primary-email').val(email);
  jQuery('#edit-primary-email-text').val(email);
  try {
    jQuery('#edit-submit-changes').click();
  }
  catch (err) {
  }
  return close_customer_select();
}

/**
 * Hide the customer selection form.
 */
function close_customer_select() {
  jQuery('#customer-select').empty().removeClass('customer-select-box');
  customer_select = '';
  return false;
}

/**
 * Prevent mistakes by confirming deletion.
 */
function confirm_line_item_delete(message, img_id) {
  if (confirm(message)) {
    var li_id = img_id.substring(3);
    jQuery('#edit-li-delete-id').val(li_id);
    jQuery('#uc-order-edit-form #edit-submit-changes').get(0).click();
  }
}

/**
 * Disable order submit button while parts of the page are still loading.
 */
function add_order_save_hold() {
  order_save_holds++;
  jQuery('#uc-order-edit-form input.save-button').attr('disabled', 'disabled');
}

/**
 * Remove a hold and enable the save buttons when all holds are gone!
 */
function remove_order_save_hold() {
  order_save_holds--;

  if (order_save_holds == 0) {
    release_held_buttons();
  }
}

/**
 * Remove the disable attribute on any input item with the save-button class.
 */
function release_held_buttons() {
  jQuery('#uc-order-edit-form input.save-button').removeAttr('disabled');
}

