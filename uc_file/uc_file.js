// $Id 

function uc_file_update_download(id, accessed, limit) {
  if (accessed < limit || limit == -1) {
    $('td#download-' + id).html(accessed + 1);
    $('td#download-' + id).attr("onclick","");
  }
}