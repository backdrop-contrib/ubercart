<?php
/**
 * Field handler.
 *
 * Provides a human-readable version of
 * the selected combination of attributes.
 */
class UcAttributeHandlerFieldOrderProductAttribute extends views_handler_field {

  /**
   * Defines a few default options for the combination field.
   */
  public function option_definition() {
    $options = parent::option_definition();

    return $options;
  }

  /**
   * {@inheritdoc}
   */
  public function render($values) {
    $data = unserialize($values->{$this->field_alias});
    $result = "";
    if (is_array($data['attributes'])) {
      $result = '';
      $rows = array();
      foreach ($data['attributes'] as $attribute => $option) {
        $rows[] = t('@attribute: @option', array(
          '@attribute' => $attribute,
          '@option' => implode(', ', (array) $option),
        ));
        if (count($rows)) {
          $variables = array(
            'items' => $rows,
            'type' => 'ul',
            'attributes' => array('class' => 'product-description'),
          );
          $result = theme('item_list', $variables);
        }
      }
    }
    return $result;
  }

}
