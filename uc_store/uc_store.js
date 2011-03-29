/**
 * @file
 * Adds click events to the order and customer tables.
 */

/**
 * Adds the double click behavior to the order table rows.
 */
Drupal.behaviors.ucCustomerOrder = {
  attach: function(context, settings) {
    jQuery('.uc-customer-table tr.odd, .uc-customer-table tr.even:not(.ucCustomerOrder-processed)', context).addClass('ucCustomerOrder-processed').each(
      function() {
        jQuery(this).dblclick(
          function() {
            window.location = settings.basePath + '?q=admin/store/customers/orders/' + this.id.substring(9);
          }
        );
      }
    );
  }
}

/**
 * Adds the double click to the customer orders table rows.
 */
Drupal.behaviors.ucCustomerOrders = {
  attach: function(context, settings) {
    jQuery('.uc-cust-orders-table tr.odd, .uc-cust-orders-table tr.even:not(.ucCustomerOrders-processed)', context).addClass('ucCustomerOrders-processed').each(
      function() {
        jQuery(this).dblclick(
          function() {
            window.location = settings.basePath + '?q=admin/store/orders/' + this.id.substring(6);
          }
        );
      }
    );
  }
}
