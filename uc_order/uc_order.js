// $Id$

var customer_select = '';
var add_product_browser = '';
var order_save_holds = 0;

// Add the double click to the order table at admin/store/orders.
$(document).ready(
  function() {
    if (order_save_holds == 0) {
      release_held_buttons();
    }

    $('.uc-orders-table tr.odd, .uc-orders-table tr.even').each(
      function() {
        $(this).dblclick(
          function() {
            var url = Drupal.settings.basePath + 'admin/store/orders/' + this.id.substring(6);
            window.location = url;
          }
        );
      }
    );

    $('#uc-order-edit-form').submit(
      function() {
        $('#products-selector').empty().removeClass();
        $('#delivery_address_select').empty().removeClass();
        $('#billing_address_select').empty().removeClass();
        $('#customer-select').empty().removeClass();
      }
    );
  }
);

/**
 * Copy the shipping data on the order edit screen to the corresponding billing
 * fields if they exist.
 */
function uc_order_copy_shipping_to_billing() {
  if ($('#edit-delivery-zone').html() != $('#edit-billing-zone').html()) {
    $('#edit-billing-zone').empty().append($('#edit-delivery-zone').children().clone());
  }

  $('#uc-order-edit-form input, select, textarea').each(
    function() {
      if (this.id.substring(0, 13) == 'edit-delivery') {
        $('#edit-billing' + this.id.substring(13)).val($(this).val());
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

  $.post(Drupal.settings.basePath + 'admin/store/orders/address_book', options,
         function (contents) {
           $(div).empty().addClass('address-select-box').append(contents);
         }
  );
}

/**
 * Apply the selected address to the appropriate fields in the order edit form.
 */
function apply_address(type, address_str) {
  eval('var address = ' + address_str + ';');
  $('#edit-' + type + '-first-name').val(address['first_name']);
  $('#edit-' + type + '-last-name').val(address['last_name']);
  $('#edit-' + type + '-phone').val(address['phone']);
  $('#edit-' + type + '-company').val(address['company']);
  $('#edit-' + type + '-street1').val(address['street1']);
  $('#edit-' + type + '-street2').val(address['street2']);
  $('#edit-' + type + '-city').val(address['city']);
  $('#edit-' + type + '-postal-code').val(address['postal_code']);

  if ($('#edit-' + type + '-country').val() != address['country']) {
    $('#edit-' + type + '-country').val(address['country']);
    try {
      uc_update_zone_select('#edit-' + type + '-country', address['zone']);
    }
    catch (err) {}
  }

  $('#edit-' + type + '-zone').val(address['zone']);
}

/**
 * Close the address book div.
 */
function close_address_select(div) {
  $(div).empty().removeClass('address-select-box');
  return false;
}

/**
 * Load the customer select div on the order edit screen.
 */
function load_customer_search() {
  if (customer_select == 'search' && $('#customer-select #edit-back').val() == null) {
    return close_customer_select();
  }

  $.post(Drupal.settings.basePath + 'admin/store/orders/customer', {},
         function (contents) {
           $('#customer-select').empty().addClass('customer-select-box').append(contents);
           $('#customer-select #edit-first-name').val($('#edit-billing-first-name').val());
           $('#customer-select #edit-last-name').val($('#edit-billing-last-name').val());
           customer_select = 'search';
         }
  );

  return false;
}

function load_customer_search_results() {
  var first_name = $('#customer-select #edit-first-name').val();
  var last_name = $('#customer-select #edit-last-name').val();
  var email = $('#customer-select #edit-email').val();

  if (first_name == '') {
    first_name = '0';
  }
  if (last_name == '') {
    last_name = '0';
  }
  if (email == '') {
    email = '0';
  }

  $.post(Drupal.settings.basePath + 'admin/store/orders/customer/search/' + first_name + '/' + last_name + '/' + email,
         { },
         function (contents) {
           $('#customer-select').empty().append(contents);
         }
  );
  return false;
}

function select_customer_search() {
  var data = $('#edit-cust-select').val();
  $('#edit-uid').val(data.substr(0, data.indexOf(':')));
  $('#edit-uid-text').val(data.substr(0, data.indexOf(':')));
  $('#edit-primary-email').val(data.substr(data.indexOf(':') + 1));
  $('#edit-primary-email-text').val(data.substr(data.indexOf(':') + 1));
  $('#edit-submit-changes').click();
  return close_customer_select();
}

function load_new_customer_form() {
  if (customer_select == 'new') {
    return close_customer_select();
  }

  $.post(Drupal.settings.basePath + 'admin/store/orders/customer/new', {},
         function (contents) {
           $('#customer-select').empty().addClass('customer-select-box').append(contents);
           customer_select = 'new';
         }
  );
  return false;
}

function check_new_customer_address() {
  var options = {
    'email' : $('#customer-select #edit-email').val(),
    'sendmail' : $('#customer-select #edit-sendmail').attr('checked')
  };
  $.post(Drupal.settings.basePath + 'admin/store/orders/customer/new/check/' + options['email'], options,
         function (contents) {
           $('#customer-select').empty().append(contents);
         }
  );
  return false;
}

function select_existing_customer(uid, email) {
  $('#edit-uid').val(uid);
  $('#edit-uid-text').val(uid);
  $('#edit-primary-email').val(email);
  $('#edit-primary-email-text').val(email);
  $('#edit-submit-changes').click();
  return close_customer_select();
}

function close_customer_select() {
  $('#customer-select').empty().removeClass('customer-select-box');
  customer_select = '';
  return false;
}

/**
 * Load the products div on the order edit screen.
 */
function uc_order_load_product_edit_div(order_id) {
  $(document).ready(
    function() {
      add_order_save_hold();

      show_product_throbber();

      $.post(Drupal.settings.basePath + 'admin/store/orders/' + order_id + '/products',
             { action: 'view' },
             function(contents) {
               if (contents != '') {
                 $('#products-container').empty().append(contents);
               }
               remove_order_save_hold();
               hide_product_throbber();
             });
    }
  );
}

function load_product_select(order_id, search) {
  if (search == true) {
    options = {'search' : $('#edit-product-search').val()};
  }
  else {
    options = { };
  }

  show_product_throbber();

  $.post(Drupal.settings.basePath + 'admin/store/orders/' + order_id + '/product_select', options,
         function (contents) {
           $('#products-selector').empty().addClass('product-select-box2').append(contents);
           hide_product_throbber();
         }
  );

  return false;
}

function select_product() {
  add_product_form();
  return false;
}

function close_product_select() {
  $('#products-selector').empty().removeClass('product-select-box2');
  return false;
}

function add_product_form() {
  add_product_browser = $('#products-selector').html();

  show_product_throbber();

  if (parseInt($('#edit-unid').val()) > 0) {
    $.post(Drupal.settings.basePath + 'admin/store/orders/' + $('#edit-order-id').val() + '/add_product/' + $('#edit-unid').val(), { },
           function(contents) {
             $('#products-selector').empty().append(contents);
             hide_product_throbber();
           }
    );
  }
}

function add_product_to_order(order_id, node_id) {
  var post_vars = fetch_product_data();
  post_vars['action'] = 'add';
  post_vars['nid'] = node_id;
  post_vars['qty'] = $('#edit-add-qty').val();

  $('#uc-order-add-product-form :input').each(
    function() {
      if ($(this).attr('name').substr(0, 10) == 'attributes') {
        post_vars[$(this).attr('name')] = $(this).val();
      }
    }
  );

  show_product_throbber();

  $.post(Drupal.settings.basePath + 'admin/store/orders/' + order_id + '/products', post_vars,
         function(contents) {
           if (contents != '') {
             $('#products-container').empty().append(contents);
           }
           hide_product_throbber();
         }
  );

  $('#add-product-button').click();

  return false;
}

function fetch_product_data() {
  var pdata = { };

  $('.order-pane-table :input').each(
    function() {
      pdata[$(this).attr('name')] = $(this).val();
    }
  );

  return pdata;
}

function add_blank_line_button(order_id) {
  var post_vars = fetch_product_data();
  post_vars['action'] = 'add_blank';

  show_product_throbber();

  $.post(Drupal.settings.basePath + 'admin/store/orders/' + order_id + '/products',
         post_vars,
         function(contents) {
           if (contents != '') {
             $('#products-container').empty().append(contents);
           }
           hide_product_throbber();
         }
  );
}

function remove_product_button(message, opid) {
  if (confirm(message)) {
    var post_vars = fetch_product_data();
    post_vars['action'] = 'remove';
    post_vars['opid'] = opid;

    show_product_throbber();

    $.post(Drupal.settings.basePath + 'admin/store/orders/' + $('#edit-order-id').val() + '/products',
           post_vars,
           function(contents) {
             if (contents != '') {
               $('#products-container').empty().append(contents);
             }
             hide_product_throbber();
           }
    );
  }
}

function confirm_line_item_delete(message, img_id) {
  if (confirm(message)) {
    var li_id = img_id.substring(3);
    $('#edit-li-delete-id').val(li_id);
    $('#uc-order-edit-form #edit-submit-changes').click();
  }
}

// Disable order submit button while parts of the page are still loading.
function add_order_save_hold() {
  order_save_holds++;
  $('#uc-order-edit-form input.save-button').attr('disabled', 'disabled');
}

// Remove a hold and enable the save buttons when all holds are gone!
function remove_order_save_hold() {
  order_save_holds--;

  if (order_save_holds == 0) {
    release_held_buttons();
  }
}

// Removes the disable attribute on any input item with the save-button class.
function release_held_buttons() {
  $('#uc-order-edit-form input.save-button').removeAttr('disabled');
}

function show_product_throbber() {
  $('#product-div-throbber').attr('style', 'background-image: url(' + Drupal.settings.basePath + 'misc/throbber.gif); background-repeat: no-repeat; background-position: 100% -20px;').html('&nbsp;&nbsp;&nbsp;&nbsp;');
}

function hide_product_throbber() {
  $('#product-div-throbber').removeAttr('style').empty();
}

