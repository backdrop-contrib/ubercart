<?php

class uc_catalog_handler_argument_term_node_tid_depth_new extends views_handler_argument_term_node_tid_depth {
  function set_breadcrumb(&$breadcrumb) {
    if (empty($this->options['set_breadcrumb']) || !is_numeric($this->argument)) {
      return;
    }

    return uc_catalog_taxonomy_set_breadcrumb($breadcrumb, $this);
  }
}