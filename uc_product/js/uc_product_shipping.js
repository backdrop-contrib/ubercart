/**
 * @file
 * Utility functions to display settings summaries on vertical tabs.
 *
 * This only gets loaded if uc_quote module is not enabled.
 */

(function ($) {

Backdrop.behaviors.ucProductShippingFieldsetSummaries = {
  attach: function (context) {
    $('fieldset.product-shipping-default', context).backdropSetSummary(function(context) {
      var vals = [];

      if ($('#edit-shippable', context).is(':checked')) {
        vals.push(Backdrop.t('Shippable'));
      }
      else {
        vals.push(Backdrop.t('Not shippable'));
      }

      return vals.join(', ');
    });
  }
};

})(jQuery);
