<?php
/**
 * @file
 * Product attribute filter handler.
 *
 * Only works for products adjustments.
 */

class UcAttributeHandlerFilterProductAttr extends views_handler_filter_in_operator {

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
    if ($this->operator != 'not in' && $this->operator != 'in') {
      return parent::query();
    }

    $this->ensure_my_table();
    $this->real_field = 'combination';
    $field = "{$this->table_alias}.{$this->real_field}";

    if (!is_array($this->value)) {
      $this->value = array($this->value);
    }

    if (!empty($this->value)) {
      if ($this->operator == 'not in') {
        $not = 'NOT';
        $condition = db_and();
      }
      else {
        $not = '';
        $condition = db_or();
      }

      foreach ($this->value as $value) {
        $value = 's:' . strlen($value) . ':"' . $value . '";';
        $condition->condition($field, "%{$value}%", "$not LIKE");
      }

      $this->query->add_where($this->options['group'], $condition);
    }
  }

}
