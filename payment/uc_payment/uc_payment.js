/**
 * @file
 * JavaScript functionality for uc_payment.module.
 */

/**
 * Pops up an info box for the credit card CVV.
 */
function cvv_info_popup() {
  var popup = window.open(Drupal.settings.ucURL.creditCardCVVInfo, 'CVV_Info', 'toolbar=0,scrollbars=1,location=0,statusbar=0,menubar=0,resizable=1,width=480,height=460,left=282,top=122');
}
