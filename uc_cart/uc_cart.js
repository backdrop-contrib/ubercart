// $Id$

var copy_box_checked = false;

/**
 * Scan the DOM and display the cancel and continue buttons.
 */
$(document).ready(
  function() {
    $('.show-onload').show();
  }
);

/**
 * When a customer clicks a Next button, expand the next pane, remove the
 * button, and don't let it collapse again.
 */                             
function uc_cart_next_button_click(button, pane_id, current) {
  if (current !== 'false') {
    $('#' + current + '-pane legend a').click();
  }
  else {
    button.disabled = true;
  }

  if ($('#' + pane_id + '-pane').attr('class').indexOf('collapsed') > -1 && $('#' + pane_id + '-pane').html() !== null) {
    $('#' + pane_id + '-pane legend a').click();
  }

  return false;
}

/**
 * Copy the delivery information to the payment information on the checkout
 * screen if corresponding fields exist.
 */
function uc_cart_copy_delivery_to_billing(checked) {
  if (!checked) {
    $('#billing-pane div.address-pane-table').slideDown();
    copy_box_checked = false;
    return false;
  }

  // Hide the billing information fields.
  $('#billing-pane div.address-pane-table').slideUp();
  copy_box_checked = true;

  // Copy over the zone options manually.
  if ($('#edit-panes-billing-billing-zone').html() != $('#edit-panes-delivery-delivery-zone').html()) {
    $('#edit-panes-billing-billing-zone').empty().append($('#edit-panes-delivery-delivery-zone').children().clone());
  }

  // Copy over the information and set it to update if delivery info changes.
  $('#delivery-pane input, select, textarea').each(
    function() {
      if (this.id.substring(0, 28) == 'edit-panes-delivery-delivery') {
        $('#edit-panes-billing-billing' + this.id.substring(28)).val($(this).val());
        $(this).change(function () { update_billing_field(this); });
      }
    }
  );

  return false;
}

function update_billing_field(field) {
  if (copy_box_checked) {
    $('#edit-panes-billing-billing' + field.id.substring(28)).val($(field).val());
  }
}

/**
 * Apply the selected address to the appropriate fields in the cart form.
 */
function apply_address(type, address_str) {
  if (address_str == '0') {
    return;
  }

  eval('var address = ' + address_str + ';');
  var temp = type + '-' + type;
  
  $('#edit-panes-' + temp + '-first-name').val(address.first_name).trigger('change');
  $('#edit-panes-' + temp + '-last-name').val(address.last_name).trigger('change');
  $('#edit-panes-' + temp + '-phone').val(address.phone).trigger('change');
  $('#edit-panes-' + temp + '-company').val(address.company).trigger('change');
  $('#edit-panes-' + temp + '-street1').val(address.street1).trigger('change');
  $('#edit-panes-' + temp + '-street2').val(address.street2).trigger('change');
  $('#edit-panes-' + temp + '-city').val(address.city).trigger('change');
  $('#edit-panes-' + temp + '-postal-code').val(address.postal_code).trigger('change');

  if ($('#edit-panes-' + temp + '-country').val() != address.country) {
    $('#edit-panes-' + temp + '-country').val(address.country);
    try {
      uc_update_zone_select('edit-panes-' + temp + '-country', address.zone);
    }
    catch (err) { }
  }

  $('#edit-panes-' + temp + '-zone').val(address.zone).trigger('change');
}
