<?php
/**
 * @file
 * Order product attribute filter handler.
 *
 * Only works for attribute with options.
 */

class UcAttributeHandlerFilterAttr extends views_handler_filter_in_operator {

  /**
   * {@inheritdoc}
   */
  public function get_value_options() {
    if (!isset($this->value_options)) {
      $aid = explode('_', $this->field);
      $aid = $aid[1];

      $this->value_title = t('Options');
      $result = db_query("SELECT name, oid FROM {uc_attribute_options} WHERE aid = :aid ORDER BY ordering", array('aid' => $aid));

      foreach ($result as $row) {
        $options[$row->oid] = $row->name;
      }
      if (count($options) == 0) {
        $options[] = t('No options found.');
      }
      $this->value_options = $options;
    }
  }

  /**
   * {@inheritdoc}
   */
  public function query() {
    $this->ensure_my_table();
    $this->real_field = 'data';
    $field = "$this->table_alias.$this->real_field";
    $not = '';

    if (!empty($this->value)) {
      if ($this->operator == 'not in') {
        $not = 'NOT';
        $condition = db_and();
      }
      else {
        $condition = db_or();
      }

      foreach ($this->value as $value) {
        $value = "i:$value;";
        $condition->condition($field, "%{$value}%", "$not LIKE");
      }

      $this->query->add_where($this->options['group'], $condition);
    }
  }

}
