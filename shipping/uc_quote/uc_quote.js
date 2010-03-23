// $Id$

/**
 * @file
 * Handle asynchronous calls on checkout page to retrieve shipping quotes.
 */

var page;
var details;
var methods;

/**
 * Set event handlers on address fields.
 */
function setQuoteCallbacks(products, context) {
  triggerQuoteCallback = function() {
    quoteCallback(products);
  };
  jQuery("input[name*=delivery_postal_code]:not(.getQuotes-processed)", context).addClass('getQuotes-processed').change(triggerQuoteCallback);
  jQuery("input[id*=quote-button]:not(.getQuotes-processed)", context).addClass('getQuotes-processed').click(function() {
    // returns false to prevent default actions and propogation
    return quoteCallback(products);
  });
  jQuery("input[name*=quote_method]:not(.getQuotes-processed)", context).addClass('getQuotes-processed').change(function() {
    // returns false to prevent default actions and propogation
    return quoteCallback(products);
  });
  jQuery("select[name*=delivery_address_select]:not(.getQuotes-processed)", context).addClass('getQuotes-processed').change(function() {
    jQuery("input[name*=delivery_postal_code]").trigger('change');
  });
  jQuery("input[name*=copy_address]:not(.getQuotes-processed)", context).addClass('getQuotes-processed').click(function() {
    if (copy_box_checked == true) {
      jQuery("input[name*=billing_postal_code]:not(.getQuotes-processed)", context).addClass('getQuotes-processed').bind('change', triggerQuoteCallback);
      jQuery("select[name*=billing_address_select]:not(.getQuotes-processed)", context).addClass('getQuotes-processed').bind('change', triggerQuoteCallback);
      triggerQuoteCallback();
    }
    else {
      jQuery("input[name*=billing_postal_code].getQuotes-processed").removeClass('getQuotes-processed').unbind('change', triggerQuoteCallback);
      jQuery("select[name*=billing_address_select].getQuotes-processed").removeClass('getQuotes-processed').unbind('change', triggerQuoteCallback);
    }
  });
}

/**
 * Refresh line item list when a shipping method is selected.
 */
function setTaxCallbacks() {
  // Choosing to use click because of IE's bloody stupid bug not to
  // trigger onChange until focus is lost. Click is better than doing
  // set_line_item() and getTax() twice, I believe.
  jQuery("#quote").find("input:radio").click(function() {
    var i = jQuery(this).val();
    if (window.set_line_item) {
      var label = jQuery(this).parent().text();
      set_line_item("shipping", label.substr(0, label.indexOf(":")), jQuery(this).parent().prev().val(), 1, 1);
    }
  });
}

/**
 * Retrieve a list of available shipping quotes.
 *
 * @param products
 *   Pipe- and carat-delimited values string representing the current contents
 *   of the shopping cart. Products are separated by | and product data by ^.
 */
function quoteCallback(products) {
  var updateCallback = function (progress, status, pb) {
    if (progress == 100) {
      pb.stopMonitoring();
    }
  };

  page = jQuery("input:hidden[name*=page]").val();
  details = new Object();
  details["uid"] = jQuery("input[name*=uid]").val();
  //details["details[zone]"] = jQuery("select[name*=delivery_zone] option:selected").val();
  //details["details[country]"] = jQuery("select[name*=delivery_country] option:selected").val();

  jQuery("select[name*=delivery_]").each(function(i) {
    details["details[delivery][" + jQuery(this).attr("name").split("delivery_")[1].replace(/]/, "") + "]"] = jQuery(this).val();
  });
  jQuery("input[name*=delivery_]").each(function(i) {
    details["details[delivery][" + jQuery(this).attr("name").split("delivery_")[1].replace(/]/, "") + "]"] = jQuery(this).val();
  });
  jQuery("select[name*=billing_]").each(function(i) {
    details["details[billing][" + jQuery(this).attr("name").split("billing_")[1].replace(/]/, "") + "]"] = jQuery(this).val();
  });
  jQuery("input[name*=billing_]").each(function(i) {
    details["details[billing][" + jQuery(this).attr("name").split("billing_")[1].replace(/]/, "") + "]"] = jQuery(this).val();
  });

  if (!!products) {
    details["products"] = products;
  }
  else {
    products = "";
    var i = 0;
    while (jQuery("input[name^='products[" + i + "]']").length) {
      products += "|" + jQuery("input[name^='products[" + i + "]']").filter("[name$='[nid]']").val();
      products += "^" + jQuery("input[name^='products[" + i + "]']").filter("[name$='[title]']").val();
      products += "^" + jQuery("input[name^='products[" + i + "]']").filter("[name$='[model]']").val();
      products += "^" + jQuery("input[name^='products[" + i + "]']").filter("[name$='[qty]']").val();
      products += "^" + jQuery("input[name^='products[" + i + "]']").filter("[name$='[cost]']").val();
      products += "^" + jQuery("input[name^='products[" + i + "]']").filter("[name$='[price]']").val();
      products += "^" + jQuery("input[name^='products[" + i + "]']").filter("[name$='[weight]']").val();
      products += "^" + jQuery("input[name^='products[" + i + "]']").filter("[name$='[data]']").val();
      i++;
    }
    details["products"] = products.substr(1);
  }
  var progress = new Drupal.progressBar("quoteProgress");
  progress.setProgress(-1, Drupal.settings.uc_quote.progress_msg);
  jQuery("#quote").empty().append(progress.element);
  jQuery("#quote").addClass("solid-border");
  // progress.startMonitoring(Drupal.settings.basePath + "?q=shipping/quote", 0);
  jQuery.ajax({
    type: "POST",
    url: Drupal.settings.ucURL.shippingQuotes,
    data: details,
    dataType: "json",
    success: displayQuote
  });

  return false;
}

/**
 * Parse and render the returned shipping quotes.
 */
function displayQuote(data) {
  var quoteDiv = jQuery("#quote").empty()/* .append("<input type=\"hidden\" name=\"method-quoted\" value=\"" + details["method"] + "\" />") */;
  var numQuotes = 0;
  var errorFlag = true;
  var i;
  for (i in data) {
    if (data[i].rate != undefined || data[i].error || data[i].notes) {
      numQuotes++;
    }
  }
  for (i in data) {
    var item = '';
    var label = data[i].option_label;
    if (data[i].rate != undefined || data[i].error || data[i].notes) {

      if (data[i].rate != undefined) {
        if (numQuotes > 1 && page != 'cart') {
          item = "<input type=\"hidden\" name=\"rate[" + i + "]\" value=\"" + data[i].rate + "\" />"
            + "<label class=\"option\">"
            + "<input type=\"radio\" class=\"form-radio\" name=\"quote-option\" value=\"" + i + "\" />"
            + label + ": " + data[i].format + "</label>";
        }
        else {
          item = "<input type=\"hidden\" name=\"quote-option\" value=\"" + i + "\" />"
            + "<input type=\"hidden\" name=\"rate[" + i + "]\" value=\"" + data[i].rate + "\" />"
            + "<label class=\"option\">" + label + ": " + data[i].format + "</label>";
          if (page == "checkout") {
            if (label != "" && window.set_line_item) {
              set_line_item("shipping", label, data[i].rate, 1);
            }
          }
        }
      }
      if (data[i].error) {
        item += '<div class="quote-error">' + data[i].error + "</div>";
      }
      if (data[i].notes) {
        item += '<div class="quote-notes">' + data[i].notes + "</div>";
      }
      if (data[i].rate == undefined && item.length) {
        item = label + ': ' + item;
      }
      quoteDiv.append('<div class="form-item">' + item + "</div>\n");
      Drupal.attachBehaviors(quoteDiv);
      if (page == "checkout") {
        // Choosing to use click because of IE's bloody stupid bug not to
        // trigger onChange until focus is lost. Click is better than doing
        // set_line_item() and getTax() twice, I believe.
        quoteDiv.find("input:radio[value=" + i +"]").click(function() {
          var i = jQuery(this).val();
          if (window.set_line_item) {
            set_line_item("shipping", data[i].option_label, data[i].rate, 1, 1);
          }
        });
      }
    }
    if (data[i].debug != undefined) {
      quoteDiv.append("<pre>" + data[i].debug + "</pre>");
    }
  }
  if (quoteDiv.find("input").length == 0) {
    quoteDiv.append(Drupal.settings.uc_quote.err_msg);
  }
  else {
    quoteDiv.find("input:radio").eq(0).click().attr("checked", "checked");
    var quoteForm = quoteDiv.html();
    quoteDiv.append("<input type=\"hidden\" name=\"quote-form\" value=\"" + Drupal.encodePath(quoteForm) + "\" />");
  }

  /* if (page == "checkout") {
    if (window.getTax) {
      getTax();
    }
    else if (window.render_line_items) {
      render_line_items();
    }
  } */
}

