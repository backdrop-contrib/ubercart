/**
 * @file
 * Utility functions to display settings summaries on vertical tabs.
 */

(function ($) {

Backdrop.behaviors.ucProductFieldsetSummaries = {
  attach: function (context) {
    $('fieldset#edit-uc-product', context).backdropSetSummary(function(context) {
      var vals = [];
      $('input:checked', context).next('label').each(function() {
        vals.push(Backdrop.checkPlain($(this).text()));
      });
      if (!$('#edit-uc-product-shippable', context).is(':checked')) {
        vals.unshift(Backdrop.t('Not shippable'));
      }
      return vals.join(', ');
    });

    $('fieldset.product-field', context).backdropSetSummary(function(context) {
      var vals = [];

      if (Backdrop.checkPlain($('#edit-model', context).val())) {
        vals.push(Backdrop.t('SKU') + ': ' + Backdrop.checkPlain($('#edit-model', context).val()));
      }

      if (Backdrop.checkPlain($('#edit-sell-price', context).val()) != '0') {
        vals.push(Backdrop.t('Sell price') + ': '
          + $('.form-item-sell-price .field-prefix', context).html()
          + Backdrop.checkPlain($('#edit-sell-price', context).val())
          + $('.form-item-sell-price .field-suffix', context).html());
      }

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
