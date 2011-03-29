/**
 * @file
 * Adds some helper JS to summaries.
 */

/**
 * Modifies the summary overviews to have onclick functionality.
 */
Drupal.behaviors.summaryOnclick = {
  attach: function(context, settings) {
    jQuery('.summary-overview:not(.summaryOnclick-processed)', context).prepend('<img src="' + settings.editIconPath + '" class="summary-edit-icon" />');

    jQuery('.summary-overview:not(.summaryOnclick-processed)', context).addClass('summaryOnclick-processed').click(function() {
      window.location = this.id;
    });
  }
}
