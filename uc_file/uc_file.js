/**
 * @file
 * Modifies the file selection and download access expiration interfaces.
 */

var uc_file_list = {};

/**
 * Disables duration amount when its type is "never".
 */
function _uc_file_expiration_disable_check(granularity, quantity) {
  // 'never' means there's no point in setting a duration.
  if (jQuery(granularity).val() == 'never') {
    jQuery(quantity).attr('disabled', 'disabled').val('');
  }
  // Anything besides 'never' should enable setting a duration.
  else {
    jQuery(quantity).removeAttr('disabled');
  }
}

/**
 * Adds files to delete to the list.
 */
function _uc_file_delete_list_populate() {
  jQuery('.affected-file-name').empty().append(uc_file_list[jQuery('#edit-recurse-directories').attr('checked')]);
}

jQuery(document).ready(
  function() {
    _uc_file_expiration_disable_check('#edit-uc-file-download-limit-duration-granularity', '#edit-uc-file-download-limit-duration-qty');
    _uc_file_expiration_disable_check('#edit-download-limit-duration-granularity', '#edit-download-limit-duration-qty');
    _uc_file_expiration_disable_check('#edit-download-limit-duration-granularity', '#edit-download-limit-duration-qty');
    _uc_file_delete_list_populate();

    toggle_limit_settings('#edit-download-override', '#edit-download-limit-number-wrapper');
    toggle_limit_settings('#edit-location-override', '#edit-download-limit-addresses-wrapper');
    toggle_limit_settings('#edit-time-override', '#edit-download-limit-duration-qty-wrapper');
    toggle_limit_settings('#edit-time-override', '#edit-download-limit-duration-granularity-wrapper');
  }
);

// When you change the global file expiration granularity select.
Drupal.behaviors.ucGlobalFileDownloadGranularity = {
  attach: function(context, settings) {
    jQuery('#edit-uc-file-download-limit-duration-granularity:not(.ucGlobalFileDownloadGranularity-processed)', context).addClass('ucGlobalFileDownloadGranularity-processed').change(
      function() {
        _uc_file_expiration_disable_check('#edit-uc-file-download-limit-duration-granularity', '#edit-uc-file-download-limit-duration-qty');
      }
    );
  }
}

// When you change the per-file expiration granularity select.
Drupal.behaviors.ucFileDownloadGranularity = {
  attach: function(context, settings) {
    jQuery('#edit-download-limit-duration-granularity:not(.ucFileDownloadGranularity-processed)', context).addClass('ucFileDownloadGranularity-processed').change(
      function() {
        _uc_file_expiration_disable_check('#edit-download-limit-duration-granularity', '#edit-download-limit-duration-qty');
      }
    );
  }
}

// When you click 'Check all' on the file action form.
Drupal.behaviors.ucFileSelectAll = {
  attach: function(context, settings) {
    jQuery('#uc_file_select_all:not(.ucFileSelectAll-processed)', context).addClass('ucFileSelectAll-processed').click(
      function() {
        jQuery('.form-checkbox').attr('checked', true);
      }
    );
  }
}

// When you click 'Uncheck all' on the file action form.
Drupal.behaviors.ucFileSelectNone = {
  attach: function(context, settings) {
    jQuery('#uc_file_select_none:not(.ucFileSelectNone-processed)', context).addClass('ucFileSelectNone-processed').click(
      function() {
        jQuery('.form-checkbox').removeAttr('checked');
      }
    );
  }
}

// When you (un)check the recursion option on the file deletion form.
Drupal.behaviors.ucFileDeleteList = {
  attach: function(context, settings) {
    jQuery('#edit-recurse-directories:not(.ucFileDeleteList-processed)', context).addClass('ucFileDeleteList-processed').change(
      function() {
        _uc_file_delete_list_populate()
      }
    );
  }
}

/**
 * Give visual feedback to the user about download numbers.
 *
 * TODO: would be to use AJAX to get the new download key and
 * insert it into the link if the user hasn't exceeded download limits.
 * I dunno if that's technically feasible though.
 */
function uc_file_update_download(id, accessed, limit) {
  if (accessed < limit || limit == -1) {

    // Handle the max download number as well.
    var downloads = '';
    downloads += accessed + 1;
    downloads += '/';
    downloads += limit == -1 ? 'Unlimited' : limit;
    jQuery('td#download-' + id).html(downloads);
    jQuery('td#download-' + id).attr("onclick", "");
  }
}

Drupal.behaviors.ucFileLimitDownloads = {
  attach: function(context, settings) {
    jQuery('#edit-download-override:not(.ucFileLimitDownloads-processed)', context).addClass('ucFileLimitDownloads-processed').click(
      function() {
        toggle_limit_settings('#edit-download-override', '#edit-download-limit-number-wrapper');
      }
    );
  }
}

Drupal.behaviors.ucFileLimitLocations = {
  attach: function(context, settings) {
    jQuery('#edit-location-override:not(.ucFileLimitLocations-processed)', context).addClass('ucFileLimitLocations-processed').click(
      function() {
        toggle_limit_settings('#edit-location-override', '#edit-download-limit-addresses-wrapper');
      }
    );
  }
}

Drupal.behaviors.ucFileLimitTime = {
  attach: function(context, settings) {
    jQuery('#edit-time-override:not(.ucFileLimitTime-processed)', context).addClass('ucFileLimitTime-processed').click(
      function() {
        toggle_limit_settings('#edit-time-override', '#edit-download-limit-duration-qty-wrapper');
        toggle_limit_settings('#edit-time-override', '#edit-download-limit-duration-granularity-wrapper');
      }
    );
  }
}

/**
 * Toggle the limit settings.
 */
function toggle_limit_settings(cause, effect) {
  if (jQuery(cause).attr('checked')) {
    jQuery(effect).show();
  }
  else {
    jQuery(effect).hide();
  }
}
