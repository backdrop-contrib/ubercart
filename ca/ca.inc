<?php
// $Id$

/**
 * @file: This files includes some test conditions and actions.
 */

// Display a message to the user.
function ca_action_drupal_set_message($settings) {
  // Get the specified input format or use the default.
  /*if (!empty($settings['message_format'])) {
    $format = $settings['message_format'];
  }
  else {
    $format = FILTER_FORMAT_DEFAULT;
  }*/

  // Filter the text using the format.
  // $message = check_markup($settings['message_text'], $format, FALSE);
  $message = check_plain($settings['message_text']);

  // Return if there's nothing to display.
  if (empty($message)) {
    return;
  }

  // Make sure we have a valid message type.
  if ($settings['message_type'] == 'error') {
    $type = $settings['message_type'];
  }
  else {
    $type = 'status';
  }

  // Output the message using the Drupal message API.
  drupal_set_message($message, $type);
}

