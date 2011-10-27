/**
 * @file
 * Adds click events to the customer and order tables.
 */

Drupal.behaviors.ucCustomerAdmin = {
  attach: function(context, settings) {
    jQuery('.uc-customer-table tbody tr', context).dblclick(
      function() {
        window.location = settings.basePath + '?q=admin/store/customers/orders/' + this.id.substring(9);
      }
    );

    jQuery('.uc-cust-orders-table tbody tr', context).dblclick(
      function() {
        window.location = settings.basePath + '?q=admin/store/orders/' + this.id.substring(6);
      }
    );
  }
}
