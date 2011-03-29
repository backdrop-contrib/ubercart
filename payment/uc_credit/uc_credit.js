/**
 * @file
 * Utility functions for displaying credit card info on the order view screen.
 */

/**
 * Initializes state.
 */
jQuery(document).ready(
  function () {
    jQuery('#cc_details_title').show(0);
    jQuery('#cc_details').hide(0);
  }
);

/**
 * Toggles credit card details.
 */
function toggle_card_details() {
  jQuery('#cc_details').toggle();
  jQuery('#cc_details_title').toggle();
}
