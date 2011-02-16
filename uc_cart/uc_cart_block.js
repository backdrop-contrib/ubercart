// $Id$

/**
 * @file
 * Adds effects and behaviors to the cart block.
 */

/**
 * Sets the behavior to (un)collapse the cart block on a click
 */
Drupal.behaviors.ucCollapseBlock = {
  attach: function(context) {
    jQuery('.cart-block-title-bar:not(.ucCollapseBlock-processed)', context).addClass('ucCollapseBlock-processed').click(
      function() {
        cart_block_toggle();
      }
    );
  }
}

/**
 * Collapses the shopping cart block at page load.
 */
jQuery(document).ready(
  function() {
    if (Drupal.settings.ucCollapsedBlock == true) {
      // Add the appropriate title bar class.
      jQuery('.cart-block-title-bar').addClass('cart-block-toggle');

      // Add the appropriate arrow class.
      jQuery('.cart-block-arrow').removeClass('arrow-down').addClass('arrow-up');
    }
    else {
      // Add the appropriate arrow class.
      jQuery('.cart-block-arrow').removeClass('arrow-up').addClass('arrow-down');
    }
  }
);

/**
 * Toggles the shopping cart block open and closed.
 */
function cart_block_toggle() {
  // Toggle the display of the cart contents table.
  jQuery('#cart-block-contents').toggle();

  // Toggle the class of the cart block arrow.
  if (jQuery('.cart-block-arrow').hasClass('arrow-up')) {
    jQuery('.cart-block-arrow').removeClass('arrow-up').addClass('arrow-down');
  }
  else {
    jQuery('.cart-block-arrow').removeClass('arrow-down').addClass('arrow-up');
  }
}
