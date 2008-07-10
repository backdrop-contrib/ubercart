// -*- js-var: set_line_item, base_path, li_titles, li_values, tax_weight; -*-
// $Id$

var pane = '';
if ($("input[@name*=delivery_]").length) {
  pane = 'delivery'
}
else if ($("input[@name*=billing_]").length) {
  pane = 'billing'
}

$(document).ready(function() {
  getTax();
  $("select[@name*=delivery_country], "
    + "select[@name*=delivery_zone], "
    + "input[@name*=delivery_postal_code], "
    + "select[@name*=billing_country], "
    + "select[@name*=billing_zone], "
    + "input[@name*=billing_postal_code]").change(getTax);
});

function getTax() {
  var products = $("[@name=cart_contents]").val();
  var s_zone = $("select[@name*=delivery_zone]").val();
  if (!s_zone) {
    s_zone = "0";
  }
  var s_code = $("input[@name*=delivery_postal_code]").val();
  if (!s_code) {
    s_code = '';
  }
  var s_country = $("select[@name*=delivery_country]").val();
  if (!s_country) {
    s_country = "0";
  }
  var b_zone = $("select[@name*=billing_zone]").val();
  if (!b_zone) {
    b_zone = "0";
  }
  var b_code = $("input[@name*=billing_postal_code]").val();
  if (!b_code) {
    b_code = '';
  }
  var b_country = $("select[@name*=billing_country]").val();
  if (!b_country) {
    b_country = "0";
  }
  var order_size = 8;
  var line_item = '';
  var key;
  var i = 0;
  for (key in li_titles) {
    if (key != 'subtotal') {
      line_item = line_item + 'i:' + i + ';a:3:{s:5:"title";s:' + li_titles[key].length + ':"' + li_titles[key] + '";s:4:"type";s:'+ key.length + ':"'+ key + '";s:6:"amount";d:' + li_values[key] + ';}';
      i++;
    }
  }
  line_item = 's:10:"line_items";a:' + i + ':{' + line_item + '}';
  var order = 'O:8:"stdClass":' + order_size + ':{s:8:"products";' + encodeURIComponent(products)
    + 's:13:"delivery_zone";i:' + s_zone
    + ';s:20:"delivery_postal_code";s:' + s_code.length +':"' + encodeURIComponent(s_code)
    + '";s:16:"delivery_country":i:' + s_country + ';'
    + 's:12:"billing_zone";i:' + b_zone
    + ';s:19:"billing_postal_code";s:' + b_code.length +':"' + encodeURIComponent(b_code)
    + '";s:15:"billing_country":i:' + b_country + ';'
    + line_item + '}';
  if (!!products) {
    $.ajax({
      type: "POST",
      url: Drupal.settings.basePath + "taxes/calculate",
      data: 'order=' + order,
      dataType: "json",
      success: function(taxes) {
        var key;
        for (key in li_titles) {
          if (key.substr(0, 4) == 'tax_') {
            delete li_titles[key];
            delete li_values[key];
            delete li_weight[key];
          }
        }
        var j;
        for (j in taxes) {
          if (taxes[j].id == 'subtotal') {
            summed = 0;
          }
          else {
            summed = 1;
          }
          set_line_item("tax_" + taxes[j].id, taxes[j].name, taxes[j].amount, tax_weight + taxes[j].weight / 10, summed, false);
        }
        render_line_items();
      }
    });
  }
}
