// -*- js-var: set_line_item, basePath, getTax; -*-
// $Id$

var page;
var details;
var methods;

function setQuoteCallbacks(products){
  $("input[@name*=delivery_postal_code]").blur(function(){
    quoteCallback(products);
  });
  $("#quote-button, input[@name*=quote_method]").click(function(){
    quoteCallback(products);
  });
  $("select[@name*=delivery_address_select]").change(function(){
    $("input[@name*=delivery_postal_code]").trigger('blur');
  });
}

function quoteCallback(products){
  var updateCallback = function (progress, status, pb) {
    if (progress == 100) {
      pb.stopMonitoring();
    }
  };

  page = $("input:hidden[@name*=page]").val();
  details = new Object();
  //details["details[zone]"] = $("select[@name*=delivery_zone] option:selected").val();
  //details["details[country]"] = $("select[@name*=delivery_country] option:selected").val();
  $("select[@name*=delivery]").each(function(i){
    details["details[" + $(this).attr("name").split("delivery_")[1].replace(/]/, "") + "]"] = $(this).children("option:selected").val();
  });
  $("input[@name*=delivery_]").each(function(i){
    details["details[" + $(this).attr("name").split("delivery_")[1].replace(/]/, "") + "]"] = $(this).val();
  });
  details["method"] = $("input:checked[@name*=quote_method], input:hidden[@name*=quote_method]").val();
  if (!!products){
    details["products"] = products;
  }
  else{
    products = "";
    var i = 0;
    var product = $("input[@name^='products[" + i + "]']")
    while (product.length){
      products += "|" + product.filter("[@name$='[nid]']").val();
      products += "^" + product.filter("[@name$='[title]']").val();
      products += "^" + product.filter("[@name$='[model]']").val();
      products += "^" + product.filter("[@name$='[manufacturer]']").val();
      products += "^" + product.filter("[@name$='[qty]']").val();
      products += "^" + product.filter("[@name$='[cost]']").val();
      products += "^" + product.filter("[@name$='[price]']").val();
      products += "^" + product.filter("[@name$='[weight]']").val();
      i++;
      product = $("input[@name^='products[" + i + "]']")
    }
    details["products"] = products.substr(1);
  }
  var progress = new Drupal.progressBar("quoteProgress");
  progress.setProgress(-1, "Receiving Quotes:");
  $("#quote").empty().append(progress.element);
  $("#quote").addClass("solid-border").css("margin-top", "1em");
  // progress.startMonitoring(basePath + "shipping/quote", 0);
  $.ajax({
    type: "POST",
    url: basePath + "?q=cart/checkout/shipping/quote",
    data: details,
    dataType: "json",
    success: displayQuote
  });
  
  return false;
}

function displayQuote(data){
  var quoteDiv = $("#quote").empty()/* .append("<input type=\"hidden\" name=\"method-quoted\" value=\"" + details["method"] + "\" />") */;
  var numQuotes = 0;
  var errorFlag = false;
  var i;
  for (i in data){
    numQuotes++;
  }
  for (i in data){
    var label = data[i].option_label;
    
    if (typeof(data[i].rate) == undefined && typeof(data[i].error) == undefined){
      errorFlag = true;
    }
    else{
      if (data[i].rate != undefined){
        if (numQuotes > 1 && page != 'cart'){
          quoteDiv.append("<div class=\"form-item\">\n"
            + "<input type=\"hidden\" name=\"rate[" + i + "]\" value=\"" + Number(data[i].rate).toFixed(2) + "\" />\n"
            + "<label class=\"option\">"
            + "<input type=\"radio\" class=\"form-radio\" name=\"quote-option\" value=\"" + i + "\" />\n"
            + label + ": " + data[i].format + "</label>\n</div>\n"
          );
          if (page == "checkout"){
            quoteDiv.find("input:radio[@value=" + i +"]").click(function(){
              var i = $(this).val();
              if (set_line_item != undefined){
                set_line_item("shipping", data[i].option_label, Number(data[i].rate).toFixed(2), 1);
              }
            }).end();
          }
        }
        else{
          quoteDiv.append("<div>\n"
            + "<input type=\"hidden\" name=\"quote-option\" value=\"" + i + "\" />\n"
            + "<input type=\"hidden\" name=\"rate[" + i + "]\" value=\"" + Number(data[i].rate).toFixed(2) + "\" />\n"
            + "<label class=\"option\">" + label + ": " + data[i].format + "</label>\n</div>\n"
          );
          if (page == "checkout"){
            if (label != "" && set_line_item != undefined){
              set_line_item("shipping", label, Number(data[i].rate).toFixed(2), 1);
            }
          }
        }
      }
      else{
        errorFlag = true;
      }
      if (data[i].debug != undefined){
        quoteDiv.append("<pre>" + data[i].debug + "</pre>");
      }
    }
  }
  if (errorFlag && quoteDiv.html().length == 0){
    quoteDiv.append("There were problems getting a shipping quote. Please verify the delivery and product information and try again.<br />If this does not resolve the issue, please call in to complete your order.");
  }
  else{
    quoteDiv.find("input:radio").eq(0).click().end().end();
    var quoteForm = quoteDiv.html();
    quoteDiv.append("<input type=\"hidden\" name=\"quote-form\" value=\"" + encodeURIComponent(quoteForm) + "\" />");
  }
  if (getTax != undefined){
    getTax();
  }
}
