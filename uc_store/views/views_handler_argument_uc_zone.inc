<?php
/**
 * @file
 * Definition of views_handler_argument_uc_zone.
 */

/**
 * Argument handler to accept a state/province code, name, or ID.
 *
 * @ingroup views_argument_handlers
 * @see views_handler_argument_uc_country
 */
class views_handler_argument_uc_zone extends views_handler_argument {

  /**
   * Overrides views_handler::option_definition().
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['zone_format'] = array(
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
    $form['zone_format'] = array(
      '#title' => t('State/province format'),
      '#description' => t('Select the format of the state/province that the argument should take. Short codes and full names are case-insensitive. Full names should be URL-encoded, i.e., spaces should be replaced by "%20".'),
      '#type' => 'select',
      '#options' => array(
        'zone_code' => t('Short code'),
        'zone_name' => t('Full name'),
        'zone_id' => t('ID'),
      ),
      '#default_value' => $this->options['zone_format']
    );
  }

  /**
   * Overrides views_handler_field::query().
   */
  public function query($group_by = FALSE) {
    $this->ensure_my_table();
    $zone_format = $this->options['zone_format'];
    $argument = urldecode(strtolower($this->argument));
    switch ($zone_format) {
      case 'zone_id':
        $table_alias = $this->table_alias;
        $field = 'zone';
        break;

      case 'zone_code':
      case 'zone_name':
        $join = new views_join();
        $join->definition = array(
          'left_table' => $this->table_alias,
          'left_field' => 'zone',
          'table' => 'uc_zones',
          'field' => 'zone_id',
          'type' => 'LEFT',
        );
        $join->construct();
        $table_alias = $this->query->add_relationship("{$this->table_alias}__uc_zones", $join, 'uc_zones');
        $field = $zone_format;
        break;
    }
    $this->query->add_where_expression(0, "LOWER({$table_alias}.{$field}) = '{$argument}'");
  }
}
