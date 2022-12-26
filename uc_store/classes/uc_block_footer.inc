<?php
/**
 * @file
 *  UcBlockFooter provides a footer text block for Ubercart.
 */
class UcBlockFooter extends Block {
  /**
   * {@inheritdoc}
   */
  function __construct($plugin_name, array $data) {
    parent::__construct($plugin_name, $data);

    $this->settings += array(
      'uc_footer_message' => 1,
    );
  }

  /**
   * Returns the rendered content of this block.
   *
   * @return string
   */
  function getContent() {
    $messages = _uc_store_footer_options();
    $id = $this->settings['uc_footer_message'];
    if ($id == 0) {
      // Pseudorandom number based on the hash of the path and the site's private
      // key, so messages are consistent between pages on the same site, but
      // different on the same pages on different sites.
      $path = backdrop_get_normal_path($_GET['q']);
      $id = (hexdec(substr(md5($path . backdrop_get_private_key()), 0, 2)) % count($messages)) + 1;
    }
    return array(
      'subject' => NULL,
      'content' => array(
        '#theme' => 'uc_store_footer',
        '#message' => $messages[$id],
      ),
    );
  }

  /**
   *  Builds the block's configuration form.
   */
  function form(&$form, &$form_state) {
    parent::form($form, $form_state);

    $form['uc_footer_message'] = array(
      '#type' => 'radios',
      '#title' => t('Footer message for store pages'),
      '#description' => t('The message selected here will appear in this block. You can place this block in the footer region of your layouts, using the visibility condition "Ubercart page" to restrict it to show only on store pages.'),
      '#options' => array_merge(
        array(0 => t('Randomly select a message from the next four options.')),
        _uc_store_footer_options()
      ),
      '#default_value' => $this->settings['uc_footer_message'],
    );
  }

  /**
   * Submit handler to save the form settings.
   */
  function formSubmit($form, &$form_state) {
    parent::formSubmit($form, $form_state);

    $this->settings['uc_footer_message'] = $form_state['values']['uc_footer_message'];
  }
}
