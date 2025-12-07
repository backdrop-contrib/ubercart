/**
 * @file
 * Utility functions to display settings summaries on vertical tabs.
 */

(function ($) {

Backdrop.behaviors.uspsAdminFieldsetSummaries = {
  attach: function (context) {
    // Credentials
    $('fieldset#edit-uc-usps-credentials', context).backdropSetSummary(function(context) {
      if ($('#edit-uc-usps-authorization-method-legacy').is(':checked')) {
        return Backdrop.t('Using legacy API');
      }
      else {
        return Backdrop.t('Using new API (OAuth2 credentials)');
      }
    });

    // USPS Domestic
    $('fieldset#edit-domestic', context).backdropSetSummary(function(context) {
      if ($('#edit-uc-usps-authorization-method-legacy').is(':checked')) {
        if ($('#edit-uc-usps-online-rates').is(':checked')) {
          return Backdrop.t('Using "online" rates');
        }
        else {
          return Backdrop.t('Using standard rates');
        }
      }
      else {
        var count_env = 0;
        $('#edit-uc-usps-newapi-dom-env-services input').each(function() {
          if ($(this).is(':checked')) {
            count_env++;
          }
        });
        var count_parcel = 0;
        $('#edit-uc-usps-newapi-dom-parcel-services input').each(function() {
          if ($(this).is(':checked')) {
            count_parcel++;
          }
        });
        return Backdrop.t('' + count_env + ' envelope services<br>' + count_parcel + ' small package services');
      }
    });

    // USPS International
    $('fieldset#edit-international', context).backdropSetSummary(function(context) {
      if ($('#edit-uc-usps-authorization-method-legacy').is(':checked')) {
        if ($('#edit-uc-usps-online-rates').is(':checked')) {
          return Backdrop.t('Using "online" rates');
        }
        else {
          return Backdrop.t('Using standard rates');
        }
      }
      else {
        var count_env = 0;
        $('#edit-uc-usps-newapi-intl-env-services input').each(function() {
          if ($(this).is(':checked')) {
            count_env++;
          }
        });
        var count_parcel = 0;
        $('#edit-uc-usps-newapi-intl-parcel-services input').each(function() {
          if ($(this).is(':checked')) {
            count_parcel++;
          }
        });
        return Backdrop.t('' + count_env + ' envelope services<br>' + count_parcel + ' small package services');
      }
    });

    // Quote options
    $('fieldset#edit-uc-usps-quote-options', context).backdropSetSummary(function(context) {
      var options;
      if ($('#edit-uc-usps-all-in-one-0').is(':checked')) {
        options = Backdrop.t('Each in own');
      }
      else {
        options = Backdrop.t('All in one');
      }
      if ($('#edit-uc-usps-authorization-method-newapi').is(':checked')) {
        options += '<br> Price type: ' + $('#edit-uc-usps-price-type', context).val().toLowerCase();
      }
      if ($('#edit-uc-usps-insurance').is(':checked')) {
        options += '<br> ' + Backdrop.t('Package insurance');
      }
      if ($('#edit-uc-usps-authorization-method-legacy').is(':checked') && $('#edit-uc-usps-delivery-confirmation').is(':checked')) {
        options += '<br> ' + Backdrop.t('Delivery confirmation');
      }
      if ($('#edit-uc-usps-signature-confirmation').is(':checked')) {
        options += '<br> ' + Backdrop.t('Signature confirmation');
      }
      return options;
    });

    // Markups
    $('fieldset#edit-uc-usps-markups', context).backdropSetSummary(function(context) {
      return Backdrop.t('Rate markup') + ': '
        + $('#edit-uc-usps-rate-markup', context).val() + ' '
        + $('#edit-uc-usps-rate-markup-type', context).val() + '<br />'
        + Backdrop.t('Weight markup') + ': '
        + $('#edit-uc-usps-weight-markup', context).val() + ' '
        + $('#edit-uc-usps-weight-markup-type', context).val();
    });

    // Debugging
    $('fieldset#edit-uc-usps-debug', context).backdropSetSummary(function(context) {
      var debug_options = [];
      if ($('#edit-uc-usps-debug-request-data').is(':checked')) {
        debug_options.push(Backdrop.t('Show request data'));
      }
      if ($('#edit-uc-usps-debug-return-data').is(':checked')) {
        debug_options.push(Backdrop.t('Show return data'));
      }
      if ($('#edit-uc-usps-debug-service-data').is(':checked')) {
        debug_options.push(Backdrop.t('Show service data'));
      }
      if ($('#edit-uc-usps-debug-show-errors').is(':checked')) {
        debug_options.push(Backdrop.t('Show errors'));
      }
      return debug_options.join('<br>');
    });
  }
};

})(jQuery);
