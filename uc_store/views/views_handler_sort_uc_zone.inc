<?php
/**
 * @file
 * Definition of views_handler_sort_uc_zone.
 */

/**
 * Handler for views_handler_sort_uc_zone, sort by state/province.
 *
 * @ingroup views_sort_handlers
 * @see views_handler_field_uc_zone
 */
class views_handler_sort_uc_zone extends views_handler_sort {

  /**
   * Overrides views_handler::option_definition().
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['zone_sort'] = array(
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
    $form['zone_sort'] = array(
      '#title' => t('Zone sort'),
      '#description' => t('Select the format that state/province should be sorted on.'),
      '#type' => 'select',
      '#options' => array(
        'zone_code' => t('Short code'),
        'zone_name' => t('Full name'),
      ),
      '#default_value' => $this->options['zone_sort']
    );
  }

  /**
   * Overrides views_handler_sort::query().
   */
  public function query() {
    $this->ensure_my_table();
    $zone_sort = $this->options['zone_sort'];
    if ($zone_sort) {
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
      $field_alias = $this->query->add_field($table_alias, $zone_sort);
      $this->query->add_orderby($table_alias, NULL, $this->options['order'], $field_alias);
    }
  }
}
