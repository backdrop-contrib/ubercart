// -*- js-var: set_line_item; -*-
// $Id$

$(document).ready(function(){
  getTax();
  $("select[@name*=delivery_zone], input[@name*=delivery_zip], select[@name*=address_select]").change(getTax);
});

function getTax(){
  var order = 'O:8:"stdClass":4:{s:8:"products";' + $("[@name=cart_contents]").val()
    + 's:13:"delivery_zone";i:' + $("select[@name*=delivery_zone] option:selected").val()
    + ';s:12:"delivery_zip";i:' + $("input[@name*=delivery_zip]").val()
    + ';s:16:"delivery_country":i:223;}';
  $.post(base_path + "taxes/" + order, null, function(tax){
    set_line_item("tax", tax_label, tax, 9);
  });
}
