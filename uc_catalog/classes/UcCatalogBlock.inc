<?php
/**
 * @file
 * Provides a configurable catalog block for Ubercart Catalog. 
 */

/**
 * UcCatalogBlock extends Block. 
 */
class UcCatalogBlock extends Block {

  /**
   * {@inheritdoc}
   */
  public function getTitle() {
    $title = parent::getTitle();

    // If no custom title, provide the default title.
    if ($this->settings['title_display'] === LAYOUT_TITLE_DEFAULT) {
      $title = t('Catalog');
    }

    // Link it to the catalog page if needed.
    if ($this->settings['uc_catalog_block_title']) {
      $title = l($title, 'catalog');
    }
    return $title;
  }

  /**
   * {@inheritdoc}
   */
  public function getAdminTitle() {
    if (!empty($this->settings['admin_label'])) {
      return check_plain($this->settings['admin_label']);
    }
    return t('Catalog');
  }

  /**
   * {@inheritdoc}
   */
  public function getContent() {
    backdrop_add_css(backdrop_get_path('module', 'uc_catalog') . '/uc_catalog.css');

    // Get the vocabulary tree information.
    $vocabulary_name = config_get('uc_catalog.settings', 'uc_catalog_vocabulary');
    $tree = taxonomy_get_tree($vocabulary_name);

    // Then convert it into an actual tree structure.
    $seq = 0;
    $menu_tree = new UcTreeNode();

    foreach ($tree as $knot) {
      $seq++;
      $knot->sequence = $seq;
      $knothole = new UcTreeNode($knot);
      // Begin at the root of the tree and find the proper place.
      $menu_tree->add_child($knothole);
    }

    // Now, create a structured menu, separate from Backdrop's menu.
    return theme('uc_catalog_block', array(
      'menu_tree' => $menu_tree,
      'uc_catalog_expand_categories' => $this->settings['uc_catalog_expand_categories'],
      'uc_catalog_block_nodecount' => $this->settings['uc_catalog_block_nodecount'],
    ));
  }
  
  /**
   * {@inheritdoc}
   */
  public function form(&$form, &$form_state) {
    parent::form($form, $form_state);
    $form['uc_catalog_block_title'] = array(
      '#type' => 'checkbox',
      '#title' => t('Make the block title a link to the top-level catalog page.'),
      '#default_value' => $this->settings['uc_catalog_block_title'],
    );
    $form['uc_catalog_expand_categories'] = array(
      '#type' => 'checkbox',
      '#title' => t('Always expand categories.'),
      '#default_value' => $this->settings['uc_catalog_expand_categories'],
    );
    $form['uc_catalog_block_nodecount'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display product counts.'),
      '#default_value' => $this->settings['uc_catalog_block_nodecount'],
    );
  }

  /**
   * {@inheritdoc}
   */
  public function formSubmit($form, &$form_state) {
    $this->settings['uc_catalog_block_title'] = $form_state['values']['uc_catalog_block_title'];
    $this->settings['uc_catalog_expand_categories'] = $form_state['values']['uc_catalog_expand_categories'];
    $this->settings['uc_catalog_block_nodecount'] = $form_state['values']['uc_catalog_block_nodecount'];
  }
}
