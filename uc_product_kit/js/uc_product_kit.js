/**
 * @file
 * Utility functions to display settings summaries on vertical tabs.
 */

(function ($) {

Backdrop.behaviors.productKitFieldsetSummaries = {
  attach: function (context) {
    $('fieldset#edit-base', context).backdropSetSummary(function(context) {
      let handled = '';
      if ($('#edit-mutable-1').is(':checked')) {
        handled = Backdrop.t('Handle as unit (no list)');
      }
      else if ($('#edit-mutable-0').is(':checked')) {
        handled = Backdrop.t('Handle as unit (w/ list)');
      }
      else {
        handled = Backdrop.t('Handle as products');
      }

      let products = [];
      $('#edit-products [selected="selected"]').each(function() {
        products.push('—' + Backdrop.checkPlain($(this).text()));
      });

      return handled + '<br>' + products.join('<br>');
    });
  }
};

})(jQuery);
