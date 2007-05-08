function switch_vocabulary(base_path, file_path){
  var settings = {
    'div': "#products-selector",
    'class': "product-ubrowserr",
    'vid': $("#ubrowser-vocab-select option:selected").val(),
    'filter': "product,kit",
    'search': "true",
    'nids': "true",
    'nodesg': "product",
    'nodepl': "products",
    'multi': "true",
    'select': "buffer_products('" + base_path +"','" + file_path +"')"
  };
  
  display_ubrowser(base_path, settings);
}

function buffer_products(base_path, file_path){
  $("#edit-unid option:selected").each(function(i){
    var productsURL = $("#edit-products").val();
    if (productsURL.search(new RegExp("\/" + this.value + "(\/|$)")) == -1){
      $("#edit-products").val(productsURL + "/" +this.value);
      $.post(base_path + '?q=products/field_image_cache/' + this.value, {}, function(contents){
        //alert(contents);
        //alert(file_path);
        if (contents != "false"){
          $("#buffer-images").append('<img src="' + file_path + '/imagecache/thumbnail/' + contents +'" />');
        }
        else{
          $("#buffer-images").append('<img src="" alt="' + $("#edit-unid option:selected").get(i).text + '" />');
        }
      });
    }
  });
}
