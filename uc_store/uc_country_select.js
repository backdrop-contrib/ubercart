/**
 * @file
 * Switches the zones list when a country is chosen for an address.
 */

/**
 * Sets the select box change behavior for the country selector
 */
Drupal.behaviors.ucCountrySelect = {
  attach: function(context, settings) {
    jQuery('select[id$=-country]:not(.ucCountrySelect-processed)', context).addClass('ucCountrySelect-processed').change(
      function() {
        uc_update_zone_select(this.id, '', settings);
      }
    );
  }
}

/**
 * Updates the zone select element with new options.
 */
function uc_update_zone_select(country_select, default_zone, settings) {
  settings = settings || Drupal.settings;

  var zone_select = country_select.substr(0, country_select.length - 8) + '-zone';

  var options = { 'country_id' : jQuery('#' + country_select).val() };

  jQuery('#' + zone_select).parent().siblings('.zone-throbber').attr('style', 'background-image: url(' + settings.basePath + 'misc/throbber.gif); background-repeat: no-repeat; background-position: 100% -20px;').html('&nbsp;&nbsp;&nbsp;&nbsp;');

  jQuery.post(settings.basePath + '?q=uc_js_util/zone_select', options,
         function (contents) {
           if (contents.match('value="-1"') != null) {
             jQuery('#' + zone_select).attr('disabled', 'disabled');
           }
           else {
             jQuery('#' + zone_select).removeAttr('disabled');
           }
           jQuery('#' + zone_select).empty().append(contents).val(default_zone).change();
           jQuery('#' + zone_select).parent().siblings('.zone-throbber').removeAttr('style').empty();
         }
  );
}
