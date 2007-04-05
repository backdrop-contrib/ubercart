// $Id$

var customer_select = '';
var add_product_browser = '';

// Add the double click to the order table at admin/store/orders.
$(document).ready(
  function() {
    $('.uc-orders-table tr.odd, .uc-orders-table tr.even').each(
      function() {
        $(this).dblclick(
          function() {
            var url = base_path + '?q=admin/store/orders/' + this.id.substring(6);
            window.location = url;
          }
        );
      }
    );
  }
);

/**
 * Copy the shipping data on the order edit screen to the corresponding billing
 * fields if they exist.
 */
function uc_order_copy_shipping_to_billing() {
  $('#uc-order-edit-form input, select, textarea').each(
    function() {
      if (this.id.substring(0, 13) == 'edit-delivery')
        $('#edit-billing' + this.id.substring(13)).val($(this).val());
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

  $.post(base_path + '?q=admin/store/orders/address_book', options,
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
  $('#edit-' + type + '-zone').val(address['zone']);
  $('#edit-' + type + '-postal-code').val(address['postal_code']);
  $('#edit-' + type + '-country').val(address['country']);
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

  $.post(base_path + '?q=admin/store/orders/customer', {},
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

  $.post(base_path + '?q=admin/store/orders/customer/search/' + first_name + '/' + last_name + '/' + email,
         { },
         function (contents) {
           $('#customer-select').empty().append(contents);
         }
  );
  return false;
}

function select_customer_search() {
  var data = $('#edit-cust-select').val();
  $('#customer-select #edit-uid').val(data.substr(0, data.indexOf(':')));
  $('#customer-select #edit-primary-email').val(data.substr(data.indexOf(':') + 1));
  return close_customer_select();
}

function load_new_customer_form() {
  if (customer_select == 'new') {
    return close_customer_select();
  }

  $.post(base_path + '?q=admin/store/orders/customer/new', {},
         function (contents) {
           $('#customer-select').empty().addClass('customer-select-box').append(contents);
           customer_select = 'new';
         }
  );
  return false;
}

function check_new_customer_address() {
  var options = {
    'email' : $('#customer-select #edit-email').val()
  };
  $.post(base_path + '?q=admin/store/orders/customer/new/check/' + options['email'], options,
         function (contents) {
           $('#customer-select').empty().append(contents);
         }
  );
  return false;
}

function select_existing_customer(uid, email) {
  $('#customer-select #edit-uid').val(uid);
  $('#customer-select #edit-primary-email').val(email);
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
      $.post(base_path + '?q=admin/store/orders/' + order_id + '/products',
             { action: 'view' },
             function(contents) {
               if (contents != '') {
                 $('#products-container').empty().append(contents);
               }
             });
    }
  );
}

function add_product_form() {
  add_product_browser = $('#products-selector').html();

  if (parseInt($('#edit-unid').val()) > 0) {
    $.post(base_path + '?q=admin/store/orders/' + $('#edit-order-id').val() + '/add_product/' + $('#edit-unid').val(), { },
           function(contents) {
             $('#products-selector').empty().append(contents);
           });
  }
}

function add_product_to_order(order_id, node_id) {
  var post_vars = {
    'action' : 'add',
    'nid'    : node_id,
    'qty'    : $('#edit-add-qty').val(),
  };
  $('#uc-order-add-product-form :input').each(
    function() {
      if ($(this).attr('name').substr(0, 10) == 'attributes') {
        post_vars[$(this).attr('name')] = $(this).val();
      }
    }
  );
  $.post(base_path + '?q=admin/store/orders/' + order_id + '/products', post_vars,
         function(contents) {
           if (contents != '') {
             $('#products-container').empty().append(contents);
           }
         }
  );
  $('#add-product-button').click();
  return false;
}

function add_blank_line_button(order_id) {
  $.post(base_path + '?q=admin/store/orders/' + order_id + '/products',
         { action: 'add_blank' },
         function(contents) {
           if (contents != '') {
             $('#products-container').empty().append(contents);
           }
         }
  );
}

function remove_product_button(message, opid) {
  if (confirm(message)) {
    $.post(base_path + '?q=admin/store/orders/' + $('#edit-order-id').val() + '/products',
           { 'action': 'remove', 'opid': opid },
           function(contents) {
             if (contents != '') {
               $('#products-container').empty().append(contents);
             }
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
