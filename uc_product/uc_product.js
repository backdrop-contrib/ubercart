// $Id$

// Functions used on the product view page.

function switch_vocabulary(vid) {
  var settings = {
    'div': "#products-selector",
    'class': "product-ubrowser",
    'vid': vid,
    'filter': $('#edit-product-filter').val(),
    'search': "true",
    'nids': "true",
    'nodesg': "product",
    'nodepl': "products",
    'multi': "true",
    'select': "buffer_products()"
  };

  display_ubrowser(Drupal.settings.basePath, settings);
}

function buffer_products(file_path) {
  $('#edit-unid option:selected').each(
    function(i) {
      if (this.value == 0) {
        return;
      }

      var productsURL = $('#edit-products').val();

      if (productsURL.search(new RegExp("\/" + this.value + "(\/|$)")) == -1) {
        if (productsURL == '') {
          $('#buffer-images').empty();
        }

        $('#edit-products').val(productsURL + '/' + this.value);
        $.post(Drupal.settings.basePath + 'products/field_image_cache/' + this.value, {},
          function(contents) {
            if (contents != 'false') {
              $('#buffer-images').append('<img src="' + file_path + '/imagecache/uc_thumbnail/' + contents +'" />');
            }
            else {
              $('#buffer-images').append('<img src="" alt="' + $('#edit-unid option:selected').get(i).text + '" />');
            }
          }
        );
      }
    }
  );
}
