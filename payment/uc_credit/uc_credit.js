/**
 * @file
 * Utility functions for displaying credit card info on the order view screen.
 */

Drupal.behaviors.ucCreditAdmin = {
  attach: function(context, settings) {
    jQuery('#cc_details_title').show();
    jQuery('#cc_details').hide();

    jQuery('#cc_details_title', context).click(
      function() {
        jQuery('#cc_details_title').hide();
        jQuery('#cc_details').show();
      }
    );
  }
}
