// $Id$

var li_titles = {};
var li_values = {};
var li_weight = {};

function set_line_item(key, title, value, weight) {
  var do_update = false;

  if (undefined === window.li_values[key]) {
    do_update = true;
  }
  else {
    if (li_titles[key] != title || li_values[key] != value || li_weight[key] != weight) {
      do_update = true;
    }
  }

  if (do_update) {
    li_titles[key] = title;
    li_values[key] = value;
    li_weight[key] = weight;

    var li_info = {};

    $.each(li_titles,
           function(a, b) {
             li_info[a] = li_weight[a] + ';' + li_values[a] + ';' + li_titles[a];
           }
    );

    $.post(base_path + '?q=cart/checkout/line_items', li_info,
           function(contents) {
             $('#line-items-div').empty().append(contents);
           }
    );
  }
}

/**
 * Display the payment details when a payment method radio button is clicked.
 */
function get_payment_details(path) {
  $.post(path, { },
    function(details) {
      if (details == '') {
        $('#payment_details').empty().addClass('display-none');
      }
      else {
        $('#payment_details').removeClass('display-none').empty().append(details);
      }
    }
  );
}

/**
 * Display the details box for the default option.
 */
function show_default_payment_details(path) {
  $('.payment-pane .form-radios .form-radio').each(
    function() {
      if (this.checked) {
        get_payment_details(path + this.value);
      }
    }
  );
}

/**
 * Pop-up an info box for the credit card CVV.
 */
function cvv_info_popup() {
  var popup = window.open(base_path + '?q=cart/checkout/credit/cvv_info', 'CVV Info', 'toolbar=0,scrollbars=1,location=0,statusbar=0,menubar=0,resizable=1,width=480,height=460,left = 282,top = 122');
}

/**
 * Toggle the payment fields on and off on the receive check form.
 */
function receive_check_toggle(checked) {
  if (!checked) {
    $('#edit-amount').removeAttr('disabled').val('');
    $('#edit-comment').removeAttr('disabled').val('');
  }
  else {
    $('#edit-amount').attr('disabled', 'true').val('-');
    $('#edit-comment').attr('disabled', 'true').val('-');
  }
}

/**
 * Return a payment method's ID from its Name.
 */
function get_payment_id(name) {
  
}
