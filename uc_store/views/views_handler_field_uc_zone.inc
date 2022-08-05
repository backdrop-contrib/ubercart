<?php
/**
 * @file
 * Definition of views_handler_field_uc_zone
 */

/**
 * Field handler to turn a state/province code into its zone name.
 *
 * @ingroup views_field_handlers
 * @see views_handler_field_uc_country
 */
class views_handler_field_uc_zone extends views_handler_field {

  /**
   * Overrides views_handler::option_definition().
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['zone_display'] = array(
      'default' => 'zone_code',
      'translatable' => TRUE,
    );
    return $options;
  }

  /**
   * Overrides views_handler::options_form().
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['zone_display'] = array(
      '#title' => t('State/province display'),
      '#description' => t('Select the way the state/province should be displayed.'),
      '#type' => 'select',
      '#options' => array(
        'zone_code' => t('Short code'),
        'zone_name' => t('Full name'),
      ),
      '#default_value' => $this->options['zone_display']
    );
  }

  /**
   * Overrides views_handler_field::query().
   */
  public function query() {
    $this->ensure_my_table();
    $zone_display = $this->options['zone_display'];
    if ($zone_display) {
      $join = new views_join();
      $join->definition = array(
        'left_table' => $this->table_alias,
        'left_field' => 'zone',
        'table' => 'uc_zones',
        'field' => 'zone_id',
        'type' => 'LEFT',
      );
      $join->construct();
      $table_alias = $this->table_alias . '__uc_zones';
      $this->query->add_relationship($table_alias, $join, 'uc_zones');
      $this->field_alias = $this->query->add_field($table_alias, $zone_display);
    }
  }
}
