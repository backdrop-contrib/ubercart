<?php
// $Id$

/**
 * @file
 * Cart administration menu items.
 */

/**
 * General settings for the shopping cart.
 *
 * @see uc_cart_cart_settings_form_validate()
 * @ingroup forms
 */
function uc_cart_cart_settings_form($form, &$form_state) {
  $form['cart-settings'] = array('#type' => 'vertical_tabs');
  
  $form['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic settings'),
    '#group' => 'cart-settings',
    '#attached' => array(
      'js' => array(
        'vertical-tabs' => drupal_get_path('module', 'uc_cart') . '/uc_cart.admin.js',
      ),
    ),
  );
  $form['general']['uc_cart_add_item_msg'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display an update message when an item is added to the cart through an add to cart form.'),
    '#default_value' => variable_get('uc_cart_add_item_msg', TRUE),
  );
  $form['general']['uc_add_item_redirect'] = array(
    '#type' => 'textfield',
    '#title' => t('Add to cart redirect'),
    '#description' => t('Enter the Drupal page to redirect to when a customer adds an item to their cart.<br />Enter &lt;none&gt; for no redirect.'),
    '#default_value' => variable_get('uc_add_item_redirect', 'cart'),
    '#size' => 32,
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
  );

  $form['general']['uc_minimum_subtotal'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum order subtotal'),
    '#description' => t('Optionally specify a minimum allowed subtotal for a cart to proceed to checkout.'),
    '#default_value' => variable_get('uc_minimum_subtotal', 0),
    '#size' => 16,
    '#field_prefix' => variable_get('uc_sign_after_amount', FALSE) ? '' : variable_get('uc_currency_sign', '$'),
    '#field_suffix' => variable_get('uc_sign_after_amount', FALSE) ? variable_get('uc_currency_sign', '$') : '',
  );

  $form['lifetime'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cart lifetime'),
    '#description' => t('Set the length of time that products remain in the cart. Cron must be running for this feature to work.'),
    '#group' => 'cart-settings',
  );

  $durations = array(
    'singular' => array(
      'minutes' => t('minute'),
      'hours' => t('hour'),
      'days' => t('day'),
      'weeks' => t('week'),
      'years' => t('year'),
    ),
    'plural' => array(
      'minutes' => t('minutes'),
      'hours' => t('hours'),
      'days' => t('days'),
      'weeks' => t('weeks'),
      'years' => t('years'),
    ),
  );

  $form['lifetime']['anonymous'] = array(
    '#type' => 'fieldset',
    '#title' => t('Anonymous users'),
  );
  $form['lifetime']['anonymous']['uc_cart_anon_duration'] = array(
    '#type' => 'select',
    '#title' => t('Duration'),
    '#options' => drupal_map_assoc(range(1, 60)),
    '#default_value' => variable_get('uc_cart_anon_duration', '4'),
    '#prefix' => '<div style="float: left; margin-right: 1em; padding-top: 0;">',
    '#suffix' => '</div>',
  );
  $form['lifetime']['anonymous']['uc_cart_anon_unit'] = array(
    '#type' => 'select',
    '#title' => t('Units'),
    '#options' => array(
      'minutes' => t('Minute(s)'),
      'hours' => t('Hour(s)'),
      'days' => t('Day(s)'),
      'weeks' => t('Week(s)'),
      'years' => t('Year(s)'),
    ),
    '#default_value' => variable_get('uc_cart_anon_unit', 'hours'),
    '#prefix' => '<div style="float: left; margin-right: 1em;">',
    '#suffix' => '</div>',
  );

  $form['lifetime']['authenticated'] = array(
    '#type' => 'fieldset',
    '#title' => t('Authenticated users'),
  );
  $form['lifetime']['authenticated']['uc_cart_auth_duration'] = array(
    '#type' => 'select',
    '#title' => t('Duration'),
    '#options' => drupal_map_assoc(range(1, 60)),
    '#default_value' => variable_get('uc_cart_auth_duration', '1'),
    '#prefix' => '<div style="float: left; margin-right: 1em; padding-top: 0;">',
    '#suffix' => '</div>',
  );
  $form['lifetime']['authenticated']['uc_cart_auth_unit'] = array(
    '#type' => 'select',
    '#title' => t('Units'),
    '#options' => array(
      'hours' => t('Hour(s)'),
      'days' => t('Day(s)'),
      'weeks' => t('Week(s)'),
      'years' => t('Year(s)'),
    ),
    '#default_value' => variable_get('uc_cart_auth_unit', 'years'),
    '#prefix' => '<div style="float: left; margin-right: 1em;">',
    '#suffix' => '</div>',
  );

  $form['continue_shopping'] = array(
    '#type' => 'fieldset',
    '#title' => t('Continue shopping element'),
    '#description' => t('Ubercart will add a continue shopping to the cart page based on the settings below.'),
    '#group' => 'cart-settings',
  );
  $form['continue_shopping']['uc_continue_shopping_type'] = array(
    '#type' => 'radios',
    '#title' => t('Continue shopping element display'),
    '#options' => array(
      'link' => t('Text link'),
      'button' => t('Button'),
      'none' => t('None'),
    ),
    '#default_value' => variable_get('uc_continue_shopping_type', 'link'),
  );
  $form['continue_shopping']['uc_continue_shopping_use_last_url'] = array(
    '#type' => 'checkbox',
    '#title' => t('Make the continue shopping link send customers back to product pages after adding items to the cart.'),
    '#description' => t('If this is disabled, the URL specified below will always be used.'),
    '#default_value' => variable_get('uc_continue_shopping_use_last_url', TRUE),
  );
  $form['continue_shopping']['uc_continue_shopping_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Default continue shopping link URL'),
    '#description' => t('Enter the Drupal page for the link to continue shopping from the cart view page.'),
    '#default_value' => variable_get('uc_continue_shopping_url', ''),
    '#size' => 32,
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
  );
  $form['continue_shopping']['uc_continue_shopping_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom continue shopping link text'),
    '#description' => t('Enter custom text for the continue shopping link.'),
    '#default_value' => variable_get('uc_continue_shopping_text', ''),
  );

  $form['breadcrumb'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cart breadcrumb'),
    '#description' => t('Drupal automatically adds a Home breadcrumb to the cart page.  You can use these settings to specify a custom breadcrumb to use on the cart page instead.  If a custom text is not supplied, the default Home breadcrumb will be displayed.'),
    '#group' => 'cart-settings',
  );
  $form['breadcrumb']['uc_cart_breadcrumb_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom cart breadcrumb text'),
    '#description' => t('Enter the text for the custom breadcrumb on the cart page.'),
    '#default_value' => variable_get('uc_cart_breadcrumb_text', ''),
  );
  $form['breadcrumb']['uc_cart_breadcrumb_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom cart breadcrumb URL'),
    '#description' => t('Enter the Drupal page linked to in the custom breadcrumb on the cart view page.'),
    '#default_value' => variable_get('uc_cart_breadcrumb_url', ''),
    '#size' => 32,
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
  );

  return system_settings_form($form);
}

/**
 * Form validation for uc_cart_cart_settings_form().
 *
 * @see uc_cart_cart_settings_form()
 */
function uc_cart_cart_settings_form_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['uc_minimum_subtotal']) || $form_state['values']['uc_minimum_subtotal'] < 0 || $form_state['values']['uc_minimum_subtotal'] === '-0') {
    form_set_error('uc_minimum_subtotal', t('Minimum order subtotal should be a non-negative number.'));
  }
}

/**
 * Settings for panes on the cart page.
 *
 * @ingroup forms
 */
function uc_cart_cart_panes_form($form, &$form_state) {
  $panes = uc_cart_cart_pane_list(NULL);

  $form['panes'] = array(
    '#theme' => 'uc_pane_sort_table',
    '#pane_prefix' => 'uc_cap',
  );
  foreach ($panes as $pane) {
    $form['panes'][$pane['id']]['uc_cap_' . $pane['id'] . '_enabled'] = array(
      '#type' => 'checkbox',
      '#default_value' => $pane['enabled'],
    );
    $form['panes'][$pane['id']]['title'] = array(
      '#markup' => $pane['title'],
    );
    $form['panes'][$pane['id']]['uc_cap_' . $pane['id'] . '_weight'] = array(
      '#type' => 'weight',
      '#delta' => 10,
      '#default_value' => $pane['weight'],
    );
  }

  return system_settings_form($form);
}

/**
 * Displays an overview of the checkout settings.
 */
function uc_cart_checkout_settings_overview() {
  // Theme all the pages beneath this path into summary overviews.
  return array(
    '#theme' => 'uc_summary_overview',
    '#summaries' => uc_summarize_child_form_pages('admin/store/settings/checkout/edit'),
  );
}

/**
 * General checkout settings.
 *
 * @ingroup forms
 */
function uc_cart_checkout_settings_form($form, &$form_state) {
  $form['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General checkout settings'),
    '#summary callback' => 'uc_summarize_form',
  );
  $form['general']['uc_checkout_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable checkout (disable to only use third party checkout service like PayPal Express Checkout).'),
    '#summary callback' => 'uc_summarize_checkbox',
    '#summary arguments' => array(
      t('Ubercart checkout is enabled.'),
      t('Ubercart checkout is disabled.'),
    ),
    '#default_value' => variable_get('uc_checkout_enabled', TRUE),
  );
  $form['general']['uc_checkout_anonymous'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable anonymous checkout (users can checkout without logging in).'),
    '#summary callback' => 'uc_summarize_checkbox',
    '#summary arguments' => array(
      t('Anonymous checkout is enabled.'),
      t('Anonymous checkout is disabled.'),
    ),
    '#default_value' => variable_get('uc_checkout_anonymous', TRUE),
  );
  $form['general']['uc_cart_delivery_not_shippable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide shipping information when possible for carts with no shippable items.'),
    '#summary callback' => 'uc_summarize_checkbox',
    '#summary arguments' => array(
      t('Shipping fields are hidden when applicable.'),
      t('Shipping fields are always shown.'),
    ),
    '#default_value' => variable_get('uc_cart_delivery_not_shippable', TRUE),
  );

  $form['pane_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Checkout pane display options'),
    '#summary callback' => 'uc_summarize_form',
  );
  $form['pane_settings']['uc_use_next_buttons'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use collapsing checkout panes with next buttons during checkout.'),
    '#summary callback' => 'uc_summarize_checkbox',
    '#summary arguments' => array(
      t('Checkout panes are collapsed with next buttons.'),
      t('Checkout panes are expanded by default.'),
    ),
    '#default_value' => variable_get('uc_use_next_buttons', FALSE),
  );
  $form['pane_settings']['uc_collapse_current_pane'] = array(
    '#type' => 'checkbox',
    '#title' => t('Collapse a pane when its next button is clicked.'),
    '#summary callback' => 'uc_summarize_checkbox',
    '#summary arguments' => array(
      t('Collapsible panes will collapse when their next buttons are clicked.'),
      t('Collapsible panes will not collapse when their next buttons are clicked.'),
    ),
    '#default_value' => variable_get('uc_collapse_current_pane', TRUE),
  );

  $form['completion'] = array(
    '#type' => 'fieldset',
    '#title' => t('Checkout completion settings'),
    '#summary callback' => 'uc_summarize_form',
  );
  $form['completion']['uc_new_customer_email'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send new customers a separate e-mail with their account details.'),
    '#summary callback' => 'uc_summarize_checkbox',
    '#summary arguments' => array(
      t('New customers receive an e-mail with their account details.'),
      t('New customers will only see their details in their initial order e-mail.')
    ),
    '#default_value' => variable_get('uc_new_customer_email', TRUE),
  );
  $form['completion']['uc_new_customer_login'] = array(
    '#type' => 'checkbox',
    '#title' => t('Login users when new customer accounts are created at checkout.'),
    '#summary callback' => 'uc_summarize_null',
    '#default_value' => variable_get('uc_new_customer_login', FALSE),
  );
  $form['completion']['uc_new_customer_status_active'] = array(
    '#type' => 'checkbox',
    '#title' => t('New customer accounts will be set to active.'),
    '#description' => t('Uncheck to create new accounts but make them blocked.'),
    '#summary callback' => 'uc_summarize_checkbox',
    '#summary arguments' => array(
      t('New customer account status will be active.'),
      t('New customer account status will be blocked.'),
    ),
    '#default_value' => variable_get('uc_new_customer_status_active', TRUE),
  );
  $form['completion']['uc_cart_checkout_complete_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Alternate checkout completion page'),
    '#description' => t('Leave blank to use the default completion page (recommended).'),
    '#summary' => variable_get('uc_cart_checkout_complete_page', '') ? t('Checkout completion page has been set to <a href="!url">!url</a>.', array('!url' => variable_get('uc_cart_checkout_complete_page', ''))) : t('Checkout completion page will be the default page.'),
    '#default_value' => variable_get('uc_cart_checkout_complete_page', ''),
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
    '#size' => 16,
  );

  return system_settings_form($form);
}

/**
 * Summarizes cart and checkout panes.
 *
 * Although this function is called 'cart_panes', it also operates on
 * checkout panes, which are structured the same way.
 *
 * @param $form
 *   The form passed from the summarizer.
 * @param $panes
 *   Any array of cart/checkout panes.
 *
 * @return
 *   An array of summary information.
 */
function _uc_cart_panes_summarize($form, $panes) {
  $items = array();

  foreach ($panes as $pane) {
    $items[] = t('@title is @enabled.', array('@title' => $pane['title'], '@enabled' => $pane['enabled'] ? t('enabled') : t('disabled')));
  }

  return $items;
}

/**
 * Settings for panes on the checkout page.
 *
 * @ingroup forms
 */
function uc_cart_checkout_panes_form($form, &$form_state) {
  $panes = _uc_checkout_pane_list();

  $form['panes'] = array(
    '#theme' => 'uc_pane_sort_table',
    '#pane_prefix' => 'uc_pane',
    '#summary callback' => '_uc_cart_panes_summarize',
    '#summary arguments' => array($panes),
  );
  foreach ($panes as $pane) {
    $form['panes'][$pane['id']]['uc_pane_' . $pane['id'] . '_enabled'] = array(
      '#type' => 'checkbox',
      '#default_value' => $pane['enabled'],
    );
    $form['panes'][$pane['id']]['title'] = array(
      '#markup' => $pane['title'],
    );
    $form['panes'][$pane['id']]['uc_pane_' . $pane['id'] . '_weight'] = array(
      '#type' => 'weight',
      '#default_value' => $pane['weight'],
    );

    $null = NULL;
    $pane_settings = $pane['callback']('settings', $null, array());
    if (is_array($pane_settings)) {
      $form['pane_' . $pane['id']] = array(
        '#type' => 'fieldset',
        '#summary callback' => 'uc_summarize_null',
        '#title' => t('!pane settings', array('!pane' => $pane['title'])),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form['pane_' . $pane['id']] = array_merge($form['pane_' . $pane['id']], $pane_settings);
    }
  }

  return system_settings_form($form);
}

/**
 * Settings for help messages displayed on the checkout page.
 *
 * @ingroup forms
 */
function uc_cart_checkout_messages_form($form, &$form_state) {
  $form['uc_checkout_instructions'] = array(
    '#type' => 'textarea',
    '#title' => t('Checkout instructions'),
    '#description' => t('Provide instructions for customers at the top of the checkout screen.'),
    '#summary' => variable_get('uc_checkout_instructions', '') ? t('Checkout instructions are set.') : t('Checkout instructions are not set.'),
    '#default_value' => variable_get('uc_checkout_instructions', ''),
    '#text_format' => variable_get('uc_checkout_instructions_format', filter_default_format()),
    '#rows' => 3,
  );

  $form['uc_checkout_review_instructions'] = array(
    '#type' => 'textarea',
    '#title' => t('Checkout review instructions'),
    '#description' => t('Provide instructions for customers at the top of the checkout review screen.'),
    '#summary' => variable_get('uc_checkout_review_instructions', uc_get_message('review_instructions'))
               ? t('Review instructions are set.')
               : t('Review instructions are not set.'),
    '#default_value' => variable_get('uc_checkout_review_instructions', uc_get_message('review_instructions')),
    '#text_format' => variable_get('uc_checkout_review_instructions_format', filter_default_format()),
    '#rows' => 3,
  );

  $form['uc_msg_order_submit'] = array(
    '#type' => 'textarea',
    '#title' => t('Checkout completion message header'),
    '#description' => t('Header for message displayed after a user checks out. <a href="!url">Uses order, store, and site tokens</a>.', array('!url' => url('admin/store/help/tokens'))),
    '#summary' => variable_get('uc_msg_order_submit', uc_get_message('completion_message'))
               ? t('Completion message is set.')
               : t('Completion message is not set.'),
    '#default_value' => variable_get('uc_msg_order_submit', uc_get_message('completion_message')),
    '#text_format' => variable_get('uc_msg_order_submit_format', filter_default_format()),
    '#rows' => 3,
  );

  $form['checkout_messages'] = array(
    '#type' => 'fieldset',
    '#title' => t('Checkout completion message body'),
    '#description' => t("In the following three boxes, you may use the special tokens !new_username for the username of a newly created account and !new_password for that account's password."),
    '#summary callback' => 'uc_summarize_form',
    '#collapsible' => FALSE,
  );
  $form['checkout_messages']['uc_msg_order_logged_in'] = array(
    '#type' => 'textarea',
    '#title' => t('Checkout completion for logged-in users'),
    '#description' => t('Message displayed upon checkout for a user who has logged in. <a href="!url">Uses order, store, and site tokens</a>.', array('!url' => url('admin/store/help/tokens'))),
    '#summary' => variable_get('uc_msg_order_logged_in', uc_get_message('completion_logged_in'))
               ? t('Completion text for logged in users is set.')
               : t('Completion text for logged in users is not set.'),
    '#default_value' => variable_get('uc_msg_order_logged_in', uc_get_message('completion_logged_in')),
    '#text_format' => variable_get('uc_msg_order_logged_in_format', filter_default_format()),
    '#rows' => 3,
  );

  $form['checkout_messages']['uc_msg_order_existing_user'] = array(
    '#type' => 'textarea',
    '#title' => t('Checkout completion for existing users'),
    '#description' => t("Message displayed upon checkout for a user who has an account but wasn't logged in. <a href=\"!url\">Uses order, store, and site tokens</a>.", array('!url' => url('admin/store/help/tokens'))),
    '#summary' => variable_get('uc_msg_order_existing_user', uc_get_message('completion_existing_user'))
               ? t('Completion text for users who aren\'t logged in is set.')
               : t('Completion text for users who aren\'t logged in is not set.'),

    '#default_value' => variable_get('uc_msg_order_existing_user', uc_get_message('completion_existing_user')),
    '#text_format' => variable_get('uc_msg_order_existing_user_format', filter_default_format()),
    '#rows' => 3,
  );

  $form['checkout_messages']['uc_msg_order_new_user'] = array(
    '#type' => 'textarea',
    '#title' => t('Checkout completion for new users'),
    '#description' => t('Message displayed upon checkout for a new user whose account was just created. <a href="!url">Uses order, store, and site tokens</a>.', array('!url' => url('admin/store/help/tokens'))),
    '#summary' => variable_get('uc_msg_order_new_user', uc_get_message('completion_new_user'))
               ? t('Completion text for new users is set.')
               : t('Completion text for new users is not set.'),
    '#default_value' => variable_get('uc_msg_order_new_user', uc_get_message('completion_new_user')),
    '#text_format' => variable_get('uc_msg_order_new_user_format', filter_default_format()),
    '#rows' => 3,
  );

  $form['uc_msg_continue_shopping'] = array(
    '#type' => 'textarea',
    '#title' => t('Continue shopping message'),
    '#description' => t('Message displayed upon checkout to direct customers to another part of your site. <a href="!url">Uses order, store, and site tokens</a>.', array('!url' => url('admin/store/help/tokens'))),
    '#summary' => variable_get('uc_msg_continue_shopping', uc_get_message('continue_shopping'))
               ? t('Continue shopping text is set.')
               : t('Continue shopping text is not set.'),
    '#default_value' => variable_get('uc_msg_continue_shopping', uc_get_message('continue_shopping')),
    '#text_format' => variable_get('uc_msg_continue_shopping_format', filter_default_format()),
    '#rows' => 3,
  );

  return system_settings_form($form);
}
