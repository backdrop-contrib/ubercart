// -*- js-var: set_line_item, base_path, li_titles, li_values; -*-
// $Id$

$(document).ready(function(){
  getTax();
  $("select[@name*=delivery_zone], input[@name*=delivery_postal_code], select[@name*=address_select]").change(getTax);
});

function getTax(){
  var order_size = 4;
  var line_item = '';
  var key;
  for (key in li_titles){
    if (key == 'shipping'){
      line_item = 's:10:"line_items";a:1:{i:0;a:2:{s:4:"type";s:8:"shipping";s:6:"amount";d:' + li_values[key] + ';}}';
      order_size = 5;
      break;
    }
  }
  var order = 'O:8:"stdClass":' + order_size + ':{s:8:"products";' + encodeURIComponent($("[@name=cart_contents]").val())
    + 's:13:"delivery_zone";i:' + $("select[@name*=delivery_zone] option:selected").val()
    + ';s:20:"delivery_postal_code";s:' + $("input[@name*=delivery_postal_code]").val().length +':"' + encodeURIComponent($("input[@name*=delivery_postal_code]").val())
  + '";s:16:"delivery_country":i:223;' + line_item + '}';
  $.ajax({
    type: "POST",
    url: base_path + "?q=taxes/" + order,
    data: null,
    dataType: "json",
    success: function(taxes){
    //if (taxes.constructor == Array){
      var i;
      for (i in taxes){
        set_line_item("tax_" + taxes[i].id, taxes[i].name, taxes[i].amount, 9);
      }
    //}
    }
  });
}
