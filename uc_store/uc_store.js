// $Id$

// Add the mouseover and mouseout functions for the store links block.
sfHover = function() {
    $('#store-links li, #store-links li li, #store-links li li li, #store-links li li li li').mouseover(
      function(){
        $(this).addClass('sfhover');
      }
    ).mouseout(
      function(){
        $(this).removeClass('sfhover');
      }
    );
  }
if (window.attachEvent) window.attachEvent("onload", sfHover); 

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
            var url = Drupal.settings('base_path') + '?q=admin/store/customers/orders/' + this.id.substring(9);
            window.location = url;
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
            var url = Drupal.settings['base_path'] + '?q=admin/store/orders/' + this.id.substring(6);
            window.location = url;
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
            window.location = Drupal.settings['base_path'] + '?q=' + this.id;
          }
        );
      }
    );
  }
);
