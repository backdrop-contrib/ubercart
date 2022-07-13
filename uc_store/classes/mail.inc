<?php
/**
 * @file
 * Utility class definition.
 */

/**
 * Modifies the Backdrop mail system to allow sending of HTML emails.
 */
class UbercartMailSystem implements MailSystemInterface {

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

  /**
   * Implements MailSystemInterface::mail().
   *
   * Retrieve the configured class responsible for delivering a message
   * defined by system.mail. Forwards the call to that class.
   */
   public function mail(array $message) {
    $mailsystem_name = config_get('system.mail', 'default-system');
    // Don't allow circular references; fall back to the core default instead.
    if ($mailsystem_name == 'UbercartMailSystem' || is_subclass_of($mailsystem_name, 'UbercartMailSystem')) {
      $mailsystem_name = 'DefaultMailSystem';
    }
    $mailsystem = new $mailsystem_name();
    return $mailsystem->mail($message);
  }
}
