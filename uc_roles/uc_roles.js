/**
 * @file
 * Enables and disables expiration fields to prevent invalid configurations.
 */

/**
 * Disables duration amount when its type is "never".
 */
function _uc_role_expiration_disable_check(granularity, quantity) {
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
 * Switches between relative and absolute expiration durations.
 */
function expiration_switcher() {
  if (jQuery('#edit-expiration').val() == 'abs') {
    jQuery("#edit-uc-roles-expire-relative-duration-wrapper").hide();
    jQuery("#edit-uc-roles-expire-relative-granularity-wrapper").hide();
    jQuery("#edit-uc-roles-by-quantity-wrapper").hide();
    jQuery("#edit-uc-roles-expire-absolute-wrapper").show();
  }
  else {
    jQuery("#edit-uc-roles-expire-absolute-wrapper").hide();
    jQuery("#edit-uc-roles-expire-relative-duration-wrapper").show();
    jQuery("#edit-uc-roles-expire-relative-granularity-wrapper").show();
    jQuery("#edit-uc-roles-by-quantity-wrapper").show();
  }
}

/**
 * Sets the default state for expiration duration.
 */
function expiration_switcher_default() {
  if (jQuery('#edit-uc-roles-default-end-expiration').val() == 'abs') {
    jQuery("#edit-uc-roles-default-length-wrapper").attr('style', 'display:none;');
    jQuery("#edit-uc-roles-default-granularity-wrapper").attr('style', 'display:none;');
    jQuery("#edit-uc-roles-default-by-quantity-wrapper").attr('style', 'display:none;');
    jQuery("#edit-uc-roles-default-end-time-wrapper").removeAttr('style');
  }
  else {
    jQuery("#edit-uc-roles-default-length-wrapper").removeAttr('style');
    jQuery("#edit-uc-roles-default-granularity-wrapper").removeAttr('style');
    jQuery("#edit-uc-roles-default-by-quantity-wrapper").removeAttr('style');
    jQuery("#edit-uc-roles-default-end-time-wrapper").attr('style', 'display:none;');
  }
}

/**
 * Overrides the expiration duration default state.
 */
function uc_roles_expiration_default_override() {
  if (jQuery('#edit-end-override').length == 0) {
    return;
  }

  if (jQuery('#edit-end-override').attr('checked')) {
    jQuery('#edit-expiration-wrapper').removeAttr('style');
    jQuery('#edit-uc-roles-expire-absolute-wrapper').removeAttr('style');
    jQuery('#edit-uc-roles-expire-relative-duration-wrapper').removeAttr('style');
    jQuery('#edit-uc-roles-expire-relative-granularity-wrapper').removeAttr('style');
    jQuery('#edit-uc-roles-by-quantity-wrapper').removeAttr('style');
    expiration_switcher();
  }
  else {
    jQuery('#edit-expiration-wrapper').attr('style', 'display:none;');
    jQuery('#edit-uc-roles-expire-relative-duration-wrapper').attr('style', 'display:none;');
    jQuery('#edit-uc-roles-expire-relative-granularity-wrapper').attr('style', 'display:none;');
    jQuery('#edit-uc-roles-by-quantity-wrapper').attr('style', 'display:none;');
    jQuery('#edit-uc-roles-expire-absolute-wrapper').attr('style', 'display:none;');
  }
}

jQuery(document).ready(
  function() {
    _uc_role_expiration_disable_check('#edit-uc-roles-expire-relative-granularity', '#edit-uc-roles-expire-relative-duration');
    _uc_role_expiration_disable_check('#edit-uc-roles-default-granularity', '#edit-uc-roles-default-length');
    _uc_role_expiration_disable_check('#edit-uc-roles-reminder-granularity', '#edit-uc-roles-reminder-length');
    uc_roles_expiration_default_override();
    expiration_switcher_default();
  }
);

// When you change the role expiration time select.
Drupal.behaviors.ucRoleExpirationTime = {
  attach: function(context, settings) {
    jQuery("#edit-expiration:not(.ucRoleExpirationTime-processed)", context).addClass('ucRoleExpirationTime-processed').change(
      function() {
        expiration_switcher();
      }
    );
  }
}

// When you change the default role expiration time select.
Drupal.behaviors.ucRoleDefaultExpirationTimeDefault = {
  attach: function(context, settings) {
    jQuery("#edit-uc-roles-default-end-expiration:not(.ucRoleDefaultExpirationTimeDefault-processed)", context).addClass('ucRoleDefaultExpirationTimeDefault-processed').change(
      function() {
        expiration_switcher_default();
      }
    );
  }
}

// When you change the role expiration time select.
Drupal.behaviors.ucRoleExpirationTimeDefault = {
  attach: function(context, settings) {
    jQuery("#edit-uc-roles-end-expiration:not(.ucRoleExpirationTimeDefault-processed)", context).addClass('ucRoleExpirationTimeDefault-processed').change(
      function() {
        expiration_switcher_default();
      }
    );
  }
}

// When you change the role expiration granularity select.
Drupal.behaviors.ucRoleExpirationGranularity = {
  attach: function(context, settings) {
    jQuery('#edit-uc-roles-expire-relative-granularity:not(.ucRoleExpirationGranularity-processed)', context).addClass('ucRoleExpirationGranularity-processed').change(
      function() {
        _uc_role_expiration_disable_check('#edit-uc-roles-expire-relative-granularity', '#edit-uc-roles-expire-relative-duration');
      }
    );
  }
}

// When you change the default role expiration granularity select.
Drupal.behaviors.ucRoleDefaultExpirationGranularity = {
  attach: function(context, settings) {
    jQuery('#edit-uc-roles-default-granularity:not(.ucRoleDefaultExpirationGranularity-processed)', context).addClass('ucRoleDefaultExpirationGranularity-processed').change(
      function() {
        _uc_role_expiration_disable_check('#edit-uc-roles-default-granularity', '#edit-uc-roles-default-length');
      }
    );
  }
}

// When you change the default role expiration granularity select.
Drupal.behaviors.ucRoleReminderExpirationGranularity = {
  attach: function(context, settings) {
    jQuery('#edit-uc-roles-reminder-granularity:not(.ucRoleReminderExpirationGranularity-processed)', context).addClass('ucRoleReminderExpirationGranularity-processed').change(
      function() {
        _uc_role_expiration_disable_check('#edit-uc-roles-reminder-granularity', '#edit-uc-roles-reminder-length');
      }
    );
  }
}

// When you change the default role expiration granularity select.
Drupal.behaviors.ucRoleExpirationEndOverride = {
  attach: function(context, settings) {
    jQuery('#edit-end-override:not(.ucRoleExpirationEndOverride-processed)', context).addClass('ucRoleExpirationEndOverride-processed').click(
      function() {
        uc_roles_expiration_default_override();
      }
    );
  }
}
