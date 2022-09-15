<?php
/**
 * @file
 * Definition of views_handler_argument_uc_country.
 */

/**
 * Argument handler to accept a state/province code, name, or ID.
 *
 * @ingroup views_argument_handlers
 * @see views_handler_argument_uc_zone
 */
class views_handler_argument_uc_country extends views_handler_argument {

  /**
   * Overrides views_handler::option_definition().
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['country_format'] = array(
      'default' => 'country_iso_code_2',
      'translatable' => TRUE,
    );
    return $options;
  }

  /**
   * Overrides views_handler::options_form().
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['country_format'] = array(
      '#title' => t('State/province format'),
      '#description' => t('Select the format of the country that the argument should take. ISO codes and full names are case-insensitive. Full names should be URL-encoded, i.e., spaces should be replaced by "%20".'),
      '#type' => 'select',
      '#options' => array(
        'country_iso_code_2' => t('2-letter ISO code'),
        'country_iso_code_3' => t('3-letter ISO code'),
        'country_name' => t('Full name'),
        'country_id' => t('ID'),
      ),
      '#default_value' => $this->options['country_format']
    );
  }

  /**
   * Overrides views_handler_field::query().
   */
  public function query($group_by = FALSE) {
    $this->ensure_my_table();
    $country_format = $this->options['country_format'];
    $argument = urldecode(strtolower($this->argument));
    switch ($country_format) {
      case 'country_id':
        $table_alias = $this->table_alias;
        $field = 'country';
        break;

      case 'country_iso_code_2':
      case 'country_iso_code_3':
      case 'country_name':
        $join = new views_join();
        $join->definition = array(
          'left_table' => $this->table_alias,
          'left_field' => 'country',
          'table' => 'uc_countries',
          'field' => 'country_id',
          'type' => 'LEFT',
        );
        $join->construct();
        $table_alias = $this->table_alias . '__uc_countries';
        $this->query->add_relationship($table_alias, $join, 'uc_countries');
        $this->field_alias = $this->query->add_field($table_alias, $country_format);
        $field = $country_format;
        break;
    }
    $this->query->add_where_expression(0, 'LOWER(' . $table_alias . '.' . $field . ")='" . $argument . "'");
  }
}
