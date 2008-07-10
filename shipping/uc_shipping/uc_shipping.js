function apply_address(type, json_address) {
  //if (json_address != "0") {
    eval("var address = " + json_address +";");
    $('input[@name=' + type + '_first_name]').val(address.first_name);
    $('input[@name=' + type + '_last_name]').val(address.last_name);
    $('input[@name=' + type + '_phone]').val(address.phone);
    $('input[@name=' + type + '_company]').val(address.company);
    $('input[@name=' + type + '_street1]').val(address.street1);
    $('input[@name=' + type + '_street2]').val(address.street2);
    $('input[@name=' + type + '_city]').val(address.city);
    $('input[@name=' + type + '_postal_code]').val(address.postal_code);
  
    if ($('select[@name=' + type + '_country]').val() != address.country) {
      $('select[@name=' + type + '_country]').val(address.country);
      try {
        uc_update_zone_select('select[@name=' + type + '_country]', address.zone);
      }
      catch (err) {}
    }
  
    $('select[@name=' + type + '_zone]').val(address.zone);
  //}
}
