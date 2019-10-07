/**
 * @file
 * Utility functions to display settings summaries on vertical tabs.
 */

(function ($) {

Backdrop.behaviors.uspsAdminFieldsetSummaries = {
  attach: function (context) {
    $('fieldset#edit-domestic', context).backdropSetSummary(function(context) {
      if ($('#edit-uc-usps-online-rates').is(':checked')) {
        return Backdrop.t('Using "online" rates');
      }
      else {
        return Backdrop.t('Using standard rates');
      }
    });

    $('fieldset#edit-uc-usps-markups', context).backdropSetSummary(function(context) {
      return Backdrop.t('Rate markup') + ': '
        + $('#edit-uc-usps-rate-markup', context).val() + ' '
        + $('#edit-uc-usps-rate-markup-type', context).val() + '<br />'
        + Backdrop.t('Weight markup') + ': '
        + $('#edit-uc-usps-weight-markup', context).val() + ' '
        + $('#edit-uc-usps-weight-markup-type', context).val();
    });
  }
};

})(jQuery);
