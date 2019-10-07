/**
 * @file
 * Utility functions to display settings summaries on vertical tabs.
 */

(function ($) {

Backdrop.behaviors.ucCartAdminFieldsetSummaries = {
  attach: function (context) {
    $('fieldset#edit-lifetime', context).backdropSetSummary(function(context) {
      return Backdrop.t('Anonymous users') + ': '
        + $('#edit-uc-cart-anon-duration', context).val() + ' '
        + $('#edit-uc-cart-anon-unit', context).val() + '<br />'
        + Backdrop.t('Authenticated users') + ': '
        + $('#edit-uc-cart-auth-duration', context).val() + ' '
        + $('#edit-uc-cart-auth-unit', context).val();
    });

    $('fieldset#edit-checkout', context).backdropSetSummary(function(context) {
      if ($('#edit-uc-checkout-enabled').is(':checked')) {
        return Backdrop.t('Checkout is enabled.');
      }
      else {
        return Backdrop.t('Checkout is disabled.');
      }
    });
    $('fieldset#edit-anonymous', context).backdropSetSummary(function(context) {
      if ($('#edit-uc-checkout-anonymous').is(':checked')) {
        return Backdrop.t('Anonymous checkout is enabled.');
      }
      else {
        return Backdrop.t('Anonymous checkout is disabled.');
      }
    });
  }
};

})(jQuery);
