// $Id$

/**
 * Scan the DOM and display the cancel and continue buttons.
 */
$(document).ready(
  function() {
    $('.show-onload').show();
  }
);

/**
 * When a customer clicks a Continue button, expand the next pane, remove the
 * button, and don't let it collapse again.
 */
function uc_cart_continue_click(button, pane_id) {
  if ($('.' + pane_id + '-pane .collapsed').html() !== null) {
    $('.' + pane_id + '-pane legend a').click();
  }
  button.disabled = true;
  return false;
}

/**
 * Copy the delivery information to the payment information on the checkout
 * screen if corresponding fields exist.
 */
function uc_cart_copy_delivery_to_billing(checked) {
  if (!checked) {
    return false;
  }

  if ($('#edit-panes-billing-pane-billing-zone').html() != $('#edit-panes-delivery-pane-delivery-zone').html()) {
    $('#edit-panes-billing-pane-billing-zone').empty().append($('#edit-panes-delivery-pane-delivery-zone').children().clone());
  }

  $('.delivery-pane input, select, textarea').each(
    function() {
      if (this.id.substring(0, 33) == 'edit-panes-delivery-pane-delivery') {
        $('#edit-panes-billing-pane-billing' + this.id.substring(33)).val($(this).val());
      }
    }
  );

  return false;
}

/**
 * Apply the selected address to the appropriate fields in the cart form.
 */
function apply_address(type, address_str) {
  eval('var address = ' + address_str + ';');
  $('#edit-panes-' + type + '-pane-' + type + '-first-name').val(address.first_name).trigger('change');
  $('#edit-panes-' + type + '-pane-' + type + '-last-name').val(address.last_name).trigger('change');
  $('#edit-panes-' + type + '-pane-' + type + '-phone').val(address.phone).trigger('change');
  $('#edit-panes-' + type + '-pane-' + type + '-company').val(address.company).trigger('change');
  $('#edit-panes-' + type + '-pane-' + type + '-street1').val(address.street1).trigger('change');
  $('#edit-panes-' + type + '-pane-' + type + '-street2').val(address.street2).trigger('change');
  $('#edit-panes-' + type + '-pane-' + type + '-city').val(address.city).trigger('change');
  $('#edit-panes-' + type + '-pane-' + type + '-postal-code').val(address.postal_code).trigger('change');

  if ($('#edit-panes-' + type + '-pane-' + type + '-country').val() != address.country) {
    $('#edit-panes-' + type + '-pane-' + type + '-country').val(address.country);
    try {
      uc_update_zone_select('#edit-panes-' + type + '-pane-' + type + '-country', address.zone);
    }
    catch (err) {}
  }

  $('#edit-panes-' + type + '-pane-' + type + '-zone').val(address.zone).trigger('change');
}
