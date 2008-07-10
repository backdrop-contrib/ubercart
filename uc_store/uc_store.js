// $Id$

// Add the show more link on the store admin display.
$(document).ready(
  function() {
    $('.uc-store-admin-panel').each(
      function() {
        var panel_id = this.id.substring(6);
        $('#show-links-' + panel_id).click(
          function() {
            var panel_id = this.id.substring(11);
            $('#panel-' + panel_id + ' .panel-links').toggle();
            if ($('#panel-' + panel_id + ' .panel-show-link').html() == '<a>' + text_show + '</a>') {
              $('#panel-' + panel_id + ' .panel-show-link').html('<a>' + text_hide + '</a>');
            }
            else {
              $('#panel-' + panel_id + ' .panel-show-link').html('<a>' + text_show + '</a>');
            }
          }
        );
      }
    )
  }
);

// Add the double click to the customer table.
$(document).ready(
  function() {
    $('.uc-customer-table tr.odd, .uc-customer-table tr.even').each(
      function() {
        $(this).dblclick(
          function() {
            window.location = Drupal.settings.basePath + 'admin/store/customers/orders/' + this.id.substring(9);
          }
        );
      }
    );
  }
);

// Add the double click to the customer orders table.
$(document).ready(
  function() {
    $('.uc-cust-orders-table tr.odd, .uc-cust-orders-table tr.even').each(
      function() {
        $(this).dblclick(
          function() {
            window.location = Drupal.settings.basePath + 'admin/store/orders/' + this.id.substring(6);
          }
        );
      }
    );
  }
);

// Add the onclick to overview section table rows.
$(document).ready(
  function() {
    $('tr.section').each(
      function() {
        $(this).click(
          function() {
            window.location = Drupal.settings.basePath + this.id;
          }
        );
      }
    );
  }
);
