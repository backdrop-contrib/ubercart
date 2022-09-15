<?php
/**
 * @file
 * Definition of views_handler_sort_uc_country.
 */

/**
 * Handler for views_handler_sort_uc_country, sort by country.
 *
 * @ingroup views_sort_handlers
 * @see views_handler_field_uc_country
 */
class views_handler_sort_uc_country extends views_handler_sort {

  /**
   * Overrides views_handler::option_definition().
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['country_sort'] = array(
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
    $form['country_sort'] = array(
      '#title' => t('Country sort'),
      '#description' => t('Select the format that countries should be sorted on.'),
      '#type' => 'select',
      '#options' => array(
        'country_name' => t('Full name'),
        'country_iso_code_2' => t('2-letter ISO code'),
        'country_iso_code_3' => t('3-letter ISO code'),
      ),
      '#default_value' => $this->options['country_sort']
    );
  }

  /**
   * Overrides views_handler_sort::query().
   */
  public function query() {
    $this->ensure_my_table();
    $country_sort = $this->options['country_sort'];
    if ($country_sort) {
      $join = new views_join();
      $join->definition = array(
        'left_table' => $this->table_alias,
        'left_field' => 'zone',
        'table' => 'uc_countries',
        'field' => 'zone_id',
        'type' => 'LEFT',
      );
      $join->construct();
      $table_alias = $this->table_alias . '__uc_countries';
      $this->query->add_relationship($table_alias, $join, 'uc_countries');
      $field_alias = $this->query->add_field($table_alias, $country_sort);
      $this->query->add_orderby($table_alias, NULL, $this->options['order'], $field_alias);
    }
  }
}
