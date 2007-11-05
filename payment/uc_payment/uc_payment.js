// $Id$

// Arrays for order total preview data.
var li_titles = {};
var li_values = {};
var li_weight = {};

// Timestamps for last time line items or payment details were updated.
var line_update = 0;
var payment_update = 0;

/**
 * Sets a line item in the order total preview.
 */
function set_line_item(key, title, value, weight) {
  var do_update = false;

  // Check to see if we're actually changing anything and need to update.
  if (window.li_values[key] === undefined) {
    do_update = true;
  }
  else {
    if (li_titles[key] != title || li_values[key] != value || li_weight[key] != weight) {
      do_update = true;
    }
  }

  if (do_update) {
    // Set the timestamp for this update.
    var this_update = new Date();

    // Set the global timestamp for the update.
    line_update = this_update.getTime();

    // Set the values passed in, overriding previous values for that key.
    li_titles[key] = title;
    li_values[key] = value;
    li_weight[key] = weight;

    // Put all the existing line item data into a single array.
    var li_info = {};
    $.each(li_titles,
      function(a, b) {
        li_info[a] = li_weight[a] + ';' + li_values[a] + ';' + li_titles[a];
      }
    );

    // Post the line item data to a URL and get it back formatted for display.
    $.post(Drupal.settings['base_path'] + 'cart/checkout/line_items', li_info,
      function(contents) {
        // Only display the changes if this was the last requested update.
        if (this_update.getTime() == line_update) {
          $('#line-items-div').empty().append(contents);
        }
      }
    );
  }
}

/**
 * Display the payment details when a payment method radio button is clicked.
 */
function get_payment_details(path) {
  var progress = new Drupal.progressBar('paymentProgress');
  progress.setProgress(-1, '');
  $('#payment_details').empty().append(progress.element).removeClass('display-none');

  // Get the timestamp for the current update.
  var this_update = new Date();

  // Set the global timestamp for the update.
  payment_update = this_update.getTime();

  // Make the post to get the details for the chosen payment method.
  $.post(path, { },
    function(details) {
      if (this_update.getTime() == payment_update) {
        // If the response was empty, throw up the default message.
        if (details == '') {
          $('#payment_details').empty().html(def_payment_msg);
        }
        // Otherwise display the returned details.
        else {
          $('#payment_details').empty().append(details);
        }
      }
    }
  );
}

/**
 * Display the details box for the default option.
 */
function show_default_payment_details(path) {
  $('#payment-pane .form-radios .form-radio').each(
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
  var popup = window.open(Drupal.settings['base_path'] + 'cart/checkout/credit/cvv_info', 'CVV_Info', 'toolbar=0,scrollbars=1,location=0,statusbar=0,menubar=0,resizable=1,width=480,height=460,left=282,top=122');
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

