/**
 * @file
 * Timestamps for last time line items or payment details were updated.
 */

var payment_update = 0;

var do_payment_details = true;

jQuery.extend(Drupal.settings, {
  ucShowProgressBar: false,
  ucOrderInitiate: false
});

jQuery(document).ready(
  function() {

    // attach a progressbar if requested
    if (Drupal.settings.ucShowProgressBar) {
      show_progressBar('#line-items-div');
    }

    // disable the submission buttons and get payment details
    if (Drupal.settings.ucOrderInitiate) {
      add_order_save_hold();
      get_payment_details(Drupal.settings.ucURL.adminOrders + jQuery('#edit-order-id').val() + '/payment_details/' + jQuery('#edit-payment-method').val());
    }
  }
)

function show_progressBar(id) {
  var progress = new Drupal.progressBar('paymentProgress');
  progress.setProgress(-1, '');
  jQuery(id).empty().append(progress.element);
}

/**
 * Doesn't refresh the payment details if they've already been loaded.
 */
function init_payment_details(payment_method) {
  if (payment_update == 0) {
    get_payment_details(Drupal.settings.ucURL.checkoutPaymentDetails + payment_method);
  }
}

/**
 * Display the payment details when a payment method radio button is clicked.
 */
function get_payment_details(path) {
  var progress = new Drupal.progressBar('paymentProgress');
  progress.setProgress(-1, '');
  jQuery('#payment_details').empty().append(progress.element).removeClass('display-none');

  // Get the timestamp for the current update.
  var this_update = new Date();

  // Set the global timestamp for the update.
  payment_update = this_update.getTime();

  var data;
  if (jQuery('#edit-payment-details-data').length) {
    data = { 'payment-details-data' : jQuery('#edit-payment-details-data').val() };
  }
  else {
    data = {};
  }
  // Make the post to get the details for the chosen payment method.
  jQuery.post(path, data,
    function(details) {
      if (this_update.getTime() == payment_update) {
        // If the response was empty, throw up the default message.
        if (details == '') {
          jQuery('#payment_details').empty().html(Drupal.settings.defPaymentMsg);
        }
        // Otherwise display the returned details.
        else {
          jQuery('#payment_details').empty().append(details);
        }
      }

      // If on the order edit screen, clear out the order save hold.
      if (window.remove_order_save_hold) {
        remove_order_save_hold();
      }
    }
  );
}

/**
 * Pops up an info box for the credit card CVV.
 */
function cvv_info_popup() {
  var popup = window.open(Drupal.settings.ucURL.creditCardCVVInfo, 'CVV_Info', 'toolbar=0,scrollbars=1,location=0,statusbar=0,menubar=0,resizable=1,width=480,height=460,left=282,top=122');
}

/**
 * Toggles the payment fields on and off on the receive check form.
 */
function receive_check_toggle(checked) {
  if (!checked) {
    jQuery('#edit-amount').removeAttr('disabled').val('');
    jQuery('#edit-comment').removeAttr('disabled').val('');
  }
  else {
    jQuery('#edit-amount').attr('disabled', 'true').val('-');
    jQuery('#edit-comment').attr('disabled', 'true').val('-');
  }
}
