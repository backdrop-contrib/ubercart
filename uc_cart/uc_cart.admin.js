// $Id$

(function ($) {

Drupal.behaviors.ucCartAdminFieldsetSummaries = {
  attach: function (context) {
    $('fieldset#edit-lifetime', context).drupalSetSummary(function(context) {
      return Drupal.t('Anonymous users') + ': '
        + $('#edit-uc-cart-anon-duration', context).val() + ' '
        + $('#edit-uc-cart-anon-unit', context).val() + '<br />'
        + Drupal.t('Authenticated users') + ': '
        + $('#edit-uc-cart-auth-duration', context).val() + ' '
        + $('#edit-uc-cart-auth-unit', context).val();
    });
  }
};

})(jQuery);
