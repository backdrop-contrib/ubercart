/**
 * @file
 * Utility functions to display settings summaries on vertical tabs.
 */

(function ($) {

Backdrop.behaviors.ucStoreAdminFieldsetSummaries = {
  attach: function (context) {

    // Don't provide info for Basic information tab because it's the default
    // and so is initially visible.

    // Store address
    $('fieldset#edit-address', context).backdropSetSummary(function(context) {
      var ret = [];
      var val = $('#edit-uc-store-street1').val();
      if (val != null && val.length > 0) {
        ret.push(val);
      }
      val = $('#edit-uc-store-street2').val();
      if (val != null && val.length > 0) {
        ret.push(val);
      }
      val = $('#edit-uc-store-city').val();
      if (val != null && val.length > 0) {
        ret.push(val);
      }
      val = $('#edit-uc-store-zone option:selected').text();
      if (val.length > 0) {
        ret.push(val);
      }
      val = $('#edit-uc-store-country option:selected').text();
      if (val.length > 0) {
        ret.push(val);
      }
      val = $('#edit-uc-store-postal-code').val();
      if (val != null && val.length > 0) {
        ret.push(val);
      }
      return ret.join('<br>');
    });

    // Currency format
    $('fieldset#edit-currency', context).backdropSetSummary(function(context) {
      var ret = [];
      var val = $('#edit-uc-currency-code').val();
      ret.push(Backdrop.t('Default currency:') + ' ' + Backdrop.checkPlain(val));
      var val = $('#edit-example').val();
      ret.push(Backdrop.t('Current format:') + ' ' + val);
      return ret.join('<br>');
    });

    // Weight format
    $('fieldset#edit-weight', context).backdropSetSummary(function(context) {
      var ret = [];
      val = $('#edit-uc-weight-unit option:selected').text();
      ret.push(val);
      return ret.join('<br>');
    });

    // Length format
    $('fieldset#edit-length', context).backdropSetSummary(function(context) {
      var ret = [];
      val = $('#edit-uc-length-unit option:selected').text();
      ret.push(val);
      return ret.join('<br>');
    });

    // Address display
    $('fieldset#edit-display', context).backdropSetSummary(function(context) {
      var ret = [];
      $('#edit-uc-customer-list-address input:checked', context).next('label').each(function() {
        ret.push(Backdrop.t('Use:') + ' ' + $(this).text());
      });
     if ($('#edit-uc-order-capitalize-addresses').is(':checked')) {
        ret.push(Backdrop.t('Capitalize addresses'));
      }
      return ret.join('<br>');
    });

    // Footer — nothing to show, this tab has no settings.
  }
};

})(jQuery);
