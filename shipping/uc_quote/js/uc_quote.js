/**
 * @file
 * Utility functions to display settings summaries on vertical tabs.
 */

(function ($) {

Backdrop.behaviors.ucQuoteFieldsetSummaries = {
  attach: function (context) {
    $('fieldset.product-shipping', context).backdropSetSummary(function(context) {
      var vals = [];

      if ($('#edit-shippable', context).is(':checked')) {
        vals.push(Backdrop.t('Shippable'));
      }
      else {
        vals.push(Backdrop.t('Not shippable'));
      }

      var shipping_type = $('#edit-shipping-type option:selected').text();
      vals.push(Backdrop.t('Default shipping type') + ': ' + Backdrop.checkPlain(shipping_type));

      var street_address = $('#edit-shipping-address-street1').val();
      if (street_address) {
        var address_type = Backdrop.t('custom');
      }
      else {
        var address_type = Backdrop.t('default');
      }
      vals.push(Backdrop.t('Pickup address') + ': ' + address_type);

      return vals.join('<br>');
    });
  }
};

})(jQuery);
