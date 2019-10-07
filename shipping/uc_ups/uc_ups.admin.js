/**
 * @file
 * Utility functions to display settings summaries on vertical tabs.
 */

(function ($) {

Backdrop.behaviors.upsAdminFieldsetSummaries = {
  attach: function (context) {
    $('fieldset#edit-uc-ups-credentials', context).backdropSetSummary(function(context) {
      var server = $('#edit-uc-ups-connection-address :selected', context).text().toLowerCase();
      return Backdrop.t('Using UPS @role server', { '@role': server });
    });

    $('fieldset#edit-uc-ups-markups', context).backdropSetSummary(function(context) {
      return Backdrop.t('Rate markup') + ': '
        + $('#edit-uc-ups-rate-markup', context).val() + ' '
        + $('#edit-uc-ups-rate-markup-type', context).val() + '<br />'
        + Backdrop.t('Weight markup') + ': '
        + $('#edit-uc-ups-weight-markup', context).val() + ' '
        + $('#edit-uc-ups-weight-markup-type', context).val();
    });

    $('fieldset#edit-uc-ups-quote-options', context).backdropSetSummary(function(context) {
      if ($('#edit-uc-ups-insurance').is(':checked')) {
        return Backdrop.t('Packages are insured');
      }
      else {
        return Backdrop.t('Packages are not insured');
      }
    });

  }
};

})(jQuery);
