// $Id$

/**
 * @file
 *   Adds some helper JS to summaries.
 */

/**
 * Modify the summary overviews to have onclick functionality.
 */
Drupal.behaviors.summaryOnclick = {
  attach: function(context, settings) {
    $('.summary-overview:not(.summaryOnclick-processed)', context).prepend('<img src="' + settings.editIconPath + '" class="summary-edit-icon" />');

    $('.summary-overview:not(.summaryOnclick-processed)', context).addClass('summaryOnclick-processed').click(function() {
      window.location = this.id;
    });
  }
}

