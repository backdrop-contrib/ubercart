/**
 * @file
 * Utility functions to display settings summaries on vertical tabs.
 */

(function ($) {

Backdrop.behaviors.creditAdminFieldsetSummaries = {
  attach: function (context) {
    $('fieldset#edit-cc-security', context).backdropSetSummary(function(context) {
      return Backdrop.t('Encryption key path') + ': '
        + $('#edit-uc-credit-encryption-path', context).val();
    });

  }
};

})(jQuery);
