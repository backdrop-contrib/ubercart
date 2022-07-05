<?php
/**
 * @file
 * Utility class definition.
 */

/**
 * Create a base class for Ubercart emails from the system config setting.
 */
$_ubercart_base_mail_system = &backdrop_static('UbercartBaseMailSystem');
if (!isset($ubercart_base_mail_system)) {
  $_ubercart_base_mail_system = config_get('system.mail', 'default-system');
  eval(sprintf('class UbercartBaseMailSystem extends %s { }', $_ubercart_base_mail_system));
}

/**
 * Modifies the Backdrop mail system to send HTML emails.
 */
class UbercartMailSystem extends UbercartBaseMailSystem {

  /**
   * Concatenates and wraps the e-mail body for plain-text mails.
   *
   * @param $message
   *   A message array, as described in hook_mail_alter().
   *
   * @return
   *   The formatted $message.
   */
  public function format(array $message) {
    $message['body'] = implode("\n\n", $message['body']);
    return $message;
  }

}
