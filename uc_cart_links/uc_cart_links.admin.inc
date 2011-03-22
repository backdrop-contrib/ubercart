<?php

/**
 * @file
 * Cart links administration menu items.
 */

/**
 * Defines a form to configure the cart links settings.
 *
 * @ingroup forms
 */
function uc_cart_links_settings_form($form, &$form_state) {
  $form['instructions'] = array(
    '#markup' => '<div>' . t('<a href="!url">View the help page</a> to learn how to create cart links.', array('!url' => url('admin/store/help/cart-links'))) . '</div>',
  );

  $form['uc_cart_links_add_show'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display the cart link product action when you add a product to your cart.'),
    '#default_value' => variable_get('uc_cart_links_add_show', FALSE),
  );
  $form['uc_cart_links_track'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track clicks through cart links that specify tracking IDs.'),
    '#default_value' => variable_get('uc_cart_links_track', TRUE),
  );
  $form['uc_cart_links_messages'] = array(
    '#type' => 'textarea',
    '#title' => t('Cart links messages'),
    '#description' => t('Enter in messages available to the cart links API for display through a link. Separate messages with a line break.<br />Messages should have a numeric key and text value. Example: 1337|Message text.'),
    '#default_value' => variable_get('uc_cart_links_messages', ''),
  );
  $form['uc_cart_links_empty'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow cart links to empty customer carts.'),
    '#default_value' => variable_get('uc_cart_links_empty', TRUE),
  );
  $form['uc_cart_links_restrictions'] = array(
    '#type' => 'textarea',
    '#title' => t('Cart links restrictions'),
    '#description' => t('To restrict what cart links may be used on your site, enter valid cart links in this textbox.  Separate links with a line break. Leave blank to permit any cart link.'),
    '#default_value' => variable_get('uc_cart_links_restrictions', ''),
  );
  $form['uc_cart_links_invalid_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Invalid link redirect page'),
    '#description' => t('Enter the URL to redirect to when an invalid cart link is used.'),
    '#default_value' => variable_get('uc_cart_links_invalid_page', ''),
    '#size' => 32,
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
  );

  return system_settings_form($form);
}

/**
 * Displays the cart links report.
 *
 * @return
 *   HTML output.
 */
function uc_cart_links_report() {
  $header = array(
    array('data' => t('ID'), 'field' => 'cart_link_id'),
    array('data' => t('Clicks'), 'field' => 'clicks'),
    array('data' => t('Last click'), 'field' => 'last_click', 'sort' => 'desc'),
  );

  $query = db_select('uc_cart_link_clicks')->extend('PagerDefault')->extend('TableSort')
    ->fields('uc_cart_link_clicks')
    ->limit(25)
    ->element(1)
    ->orderByHeader($header);

  $rows = array();
  $result = $query->execute();
  foreach ($result as $data) {
    $rows[] = array(
      check_plain($data->cart_link_id),
      $data->clicks,
      format_date($data->last_click, 'short'),
    );
  }

  if (empty($rows)) {
    $rows[] = array(
      array('data' => t('No cart links have been tracked yet.'), 'colspan' => 3),
    );
  }

  $build['report'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  $build['pager'] = array(
    '#theme' => 'pager',
    '#element' => 1,
  );

  return $build;
}

/**
 * Learn how to create cart links for your products.
 *
 * @return
 *   Help text in HTML format.
 */
function uc_cart_links_creation_help() {
  $items = array(
    t('The cart link should be /cart/add/cart_link_content.'),
    t('Chain together as many actions as you want with dashes.'),
    t('Do not put any spaces or use dashes in any action arguments.'),
    t('Use the table below to learn about actions and their arguments.'),
    t('Arguments come directly after their action letters.'),
    t('Specify the redirection by adding ?destination=url where url is the page to go to.'),
  );
  $header = array(t('Action'), t('Description'), t('Argument'));
  $rows = array(
    array('i', t('Sets the ID of the cart link.'), t('A custom text ID for the link.')),
    array('m', t('Displays a preset message to the customer.'), t('A message ID.')),
    array('p', t('Adds a product to the cart.'), t('A product string using the rules below...')),
  );
  $items2 = array(
    t("You must at least specify a node ID immediately after the 'p'."),
    t('Add additional specifications separated by underscores.'),
    t('Specify the quantity with q followed by the number to add.'),
    t('Specify attributes/options using a#o#, replacing # with the ID of the attribute and option.'),
    t('Turn off the add to cart message with m0.'),
  );

  $build = array(
    '#prefix' => '<div>',
    '#suffix' => '</div>',
  );
  $build['introduction'] = array('#markup' => t('There is currently no user interface for creating cart links, but this section includes some basic instructions.<br />Cart links are simple to form using a few actions and arguments with the following rules:'));
  $build['suggestions'] = array(
    '#theme' => 'item_list',
    '#items' => $items,
  );
  $build['commands'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  $build['rules'] = array(
    '#theme' => 'item_list',
    '#items' => $items2,
  );
  $build['example'] = array('#markup' => '<p>' . t('<b>Example:</b> /cart/add/p1_q5-imonday_special?destination=cart<br /><br />This example will add 5 of product 1 to the cart, track clicks with the ID "monday_special", and redirect the user to the cart. To use this on your site, simply create an HTML link to the URL you create:') . '</p><p>&lt;a href="http://www.example.com/cart/add/p1_q5-imonday_special?destination=cart"&gt;' . t('Link text.') . '&lt;/a&gt;</p>');

  return $build;
}
