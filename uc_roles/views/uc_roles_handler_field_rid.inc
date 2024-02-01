<?php
/**
 * @file
 * Views handler: Role ID field.
 */

/**
 * Returns a role id rendered as a role name to display in the View.
 */
class uc_roles_handler_field_rid extends views_handler_field {

  /**
   * Overrides views_handler_field::pre_render().
   */
  public function pre_render(&$values) {
    // Get a list of all the role names.
    if (!isset($this->user_roles)) {
      $this->user_roles = user_roles(TRUE);
    }
  }

  /**
   * Overrides views_handler_field::render().
   */
  public function render($values) {
    // Return the role name corresponding to the role ID.
    $role = $values->uc_roles_expirations_role;
    if ($role) {
      return isset($this->user_roles[$role]) ? $this->user_roles[$role] : $role;
    }
  }
}
