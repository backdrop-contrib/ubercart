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
      var ret = [];
      if ($('#edit-uc-checkout-enabled').is(':checked')) {
        ret.push(Backdrop.t('Checkout is enabled'));
      }
      else {
        ret.push(Backdrop.t('Checkout is disabled'));
      }
      return ret.join('<br>');
    });
    $('fieldset#edit-anonymous', context).backdropSetSummary(function(context) {
      var ret = [];
      if ($('#edit-uc-checkout-anonymous').is(':checked')) {
        ret.push(Backdrop.t('Anonymous checkout is enabled'));
      }
      else {
        ret.push(Backdrop.t('Anonymous checkout is disabled'));
      }
      return ret.join('<br>');
    });
    $('fieldset#edit-pane-customer', context).backdropSetSummary(function(context) {
      if ($('#edit-uc-cart-new-account-details').val().length) {
        return Backdrop.t('Help message set');
      }
      else {
        return Backdrop.t('Help message empty');
      }
    });
    $('fieldset#edit-pane-payment', context).backdropSetSummary(function(context) {
      if ($('#edit-uc-payment-show-order-total-preview').is(':checked')) {
        return Backdrop.t('Show order preview');
      }
      else {
        return Backdrop.t('No order preview');
      }
    });
    $('fieldset#edit-instructions', context).backdropSetSummary(function(context) {
      var ret = [];
      if ($('#edit-uc-checkout-instructions').val().length) {
        ret.push(Backdrop.t('Checkout instructions set'));
      }
      else {
        ret.push(Backdrop.t('No checkout instructions'));
      }
      if ($('#edit-uc-checkout-review-instructions').val().length) {
        ret.push(Backdrop.t('Review instructions set'));
      }
      else {
        ret.push(Backdrop.t('No review instructions'));
      }
      return ret.join('<br>');
    });

    $('fieldset#edit-completion-messages', context).backdropSetSummary(function(context) {
      var ret = [];
      var completion_page = $('#edit-uc-cart-checkout-complete-page').val();
      var completion_text = Backdrop.t('Completion page:') + ' ';
      if (completion_page.length) {
        ret.push(completion_text + Backdrop.checkPlain(completion_page));
      }
      else {
        ret.push(completion_text + Backdrop.t('&lt;default&gt;'));
      }
      var set_msgs = 0;
      var empty_msgs = 0;
      $('textarea', context).each(function() {
        if ($(this).val().length) {
          set_msgs++;
        }
        else {
          empty_msgs++;
        }
      });
      ret.push(set_msgs + ' ' + Backdrop.t('messages set,') + ' ' + empty_msgs + ' ' + Backdrop.t('messages empty'));
      return ret.join('<br>');
    });
  }
};

})(jQuery);
