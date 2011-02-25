<?php

/**
 * @file
 * Token hooks.
 */

/**
 * Implements hook_token_info().
 */
function uc_file_token_info() {
  $type = array(
    'name' => t('File downloads'),
    'description' => t('Tokens for purchased file downloads.'),
    'needs-data' => 'uc_file',
  );

  $files['downloads'] = array(
    'name' => t('Downloads'),
    'description' => t('The list of file download links (if any) associated with an order'),
  );

  return array(
    'types' => array('uc_file' => $type),
    'tokens' => array('uc_file' => $files),
  );
}

/**
 * Implements hook_tokens().
 */
function uc_file_tokens($type, $tokens, $data = array(), $options = array()) {
  $values = array();

  if ($type == 'uc_file' && !empty($data['uc_file'])) {
    $files = $data['uc_file'];

    if (isset($tokens['downloads'])) {
      $values[$tokens['downloads']] = theme('uc_file_downloads_token', array('file_downloads' => $files));
    }
  }

  return $values;
}

/**
 * Theme file download links token.
 *
 * @ingroup themeable
 */
function theme_uc_file_downloads_token($variables) {
  $file_downloads = $variables['file_downloads'];

  $output = '';

  foreach ($file_downloads as $file_download) {
    // Let's only notify of them of the files, not the directories.
    if (is_dir($file_download->filename)) {
      continue;
    }

    $output .= l($file_download->filename, 'download/' . $file_download->fid . '/' . $file_download->file_key, array('absolute' => TRUE)) . "\n";
  }

  return $output;
}
