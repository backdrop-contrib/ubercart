/**
 * @file
 * Utility functions to display settings summaries on vertical tabs.
 */

(function ($) {

Backdrop.behaviors.ucCartAdminFieldsetSummaries = {
  attach: function (context) {

    // Cart settings
    $('fieldset#edit-lifetime', context).backdropSetSummary(function(context) {
      return Backdrop.t('Anonymous users') + ': '
        + $('#edit-uc-cart-anon-duration', context).val() + ' '
        + $('#edit-uc-cart-anon-unit', context).val() + '<br />'
        + Backdrop.t('Authenticated users') + ': '
        + $('#edit-uc-cart-auth-duration', context).val() + ' '
        + $('#edit-uc-cart-auth-unit', context).val();
    });
    $('fieldset#edit-continue-shopping', context).backdropSetSummary(function(context) {
      var ret = [];
      $('#edit-uc-continue-shopping-type input:checked', context).next('label').each(function() {
        ret.push(Backdrop.t('Use:') + ' ' + $(this).text());
      });
      if ($('#edit-uc-continue-shopping-use-last-url').is(':checked')) {
        ret.push(Backdrop.t('Go back to last item'));
      }
      var continue_url = $('#edit-uc-continue-shopping-url').val();
      ret.push(Backdrop.t('Destination:') + ' ' + Backdrop.checkPlain(continue_url));
      return ret.join('<br>');
    });
    $('fieldset#edit-breadcrumb', context).backdropSetSummary(function(context) {
      var ret = [];
      var breadcrumb_text = $('#edit-uc-cart-breadcrumb-text').val();
      var breadcrumb_label = Backdrop.t('Breadcrumb text:') + ' ';
      if (breadcrumb_text.length) {
        breadcrumb_label += Backdrop.checkPlain(breadcrumb_text);
      }
      else {
        breadcrumb_label += Backdrop.t('Default <em>Home</em>');
      }
      ret.push(breadcrumb_label);

      var breadcrumb_url = $('#edit-uc-cart-breadcrumb-url').val();
      ret.push(Backdrop.t('Breadcrumb destination:') + ' ' + Backdrop.checkPlain(breadcrumb_url));

      return ret.join('<br>');
    });

    // Checkout settings
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
