<?php
/**
 * @file
 * Definition of views_handler_field_uc_country
 */

/**
 * Field handler to turn a numeric country code into its name.
 *
 * @ingroup views_field_handlers
 * @see views_handler_field_uc_zone
 */
class views_handler_field_uc_country extends views_handler_field {

  /**
   * Overrides views_handler::option_definition().
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['country_display'] = array(
      'default' => 'country_name',
      'translatable' => TRUE,
    );
    return $options;
  }

  /**
   * Overrides views_handler::options_form().
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['country_display'] = array(
      '#title' => t('Country display'),
      '#description' => t('Select the way the country should be displayed.'),
      '#type' => 'select',
      '#options' => array(
        'country_name' => t('Full name'),
        'country_iso_code_2' => t('2-letter ISO code'),
        'country_iso_code_3' => t('3-letter ISO code'),
      ),
      '#default_value' => $this->options['country_display']
    );
  }

  /**
   * Overrides views_handler_field::query().
   */
  public function query() {
    $this->ensure_my_table();
    $country_display = $this->options['country_display'];
    if ($country_display) {
      $join = new views_join();
      $join->definition = array(
        'left_table' => $this->table_alias,
        'left_field' => 'country',
        'table' => 'uc_countries',
        'field' => 'country_id',
        'type' => 'LEFT',
      );
      $join->construct();
      $table_alias = $this->query->add_relationship("{$this->table_alias}__uc_countries", $join, 'uc_countries');
      $this->field_alias = $this->query->add_field($table_alias, $country_display);
    }
  }
}
