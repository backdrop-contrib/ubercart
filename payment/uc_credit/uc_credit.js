// $Id$

jQuery(document).ready(
  function () {
    jQuery('#cc_details_title').show(0);
    jQuery('#cc_details').hide(0);
  }
);

/**
 * Toggles credit card details on the order view screen.
 */
function toggle_card_details() {
  jQuery('#cc_details').toggle();
  jQuery('#cc_details_title').toggle();
}

