<?php

/**
 * @file
 * This file is the default customer invoice template for Ubercart.
 *
 * Available variables:
 * - $products: An array of product objects in the order, with the following
 *   members:
 *   - title: The product title.
 *   - model: The product SKU.
 *   - qty: The quantity ordered.
 *   - total_price: The formatted total price for the quantity ordered.
 *   - individual_price: If quantity is more than 1, the formatted product
 *     price of a single item.
 *   - details: Any extra details about the product, such as attributes.
 * - $line_items: An array of line item arrays attached to the order, each with
 *   the following keys:
 *   - line_item_id: The type of line item (subtotal, shipping, etc.).
 *   - title: The line item display title.
 *   - formatted_amount: The formatted amount of the line item.
 * - $shippable: TRUE if the order is shippable.
 *
 * Tokens: All site, store and order tokens are also available as
 * variables, such as $site_logo, $store_name and $order_first_name.
 *
 * Display options:
 * - $op: 'view', 'print', 'checkout-mail' or 'admin-mail', depending on
 *   which variant of the invoice is being rendered.
 * - $business_header: TRUE if the invoice header should be displayed.
 * - $shipping_method: TRUE if shipping information should be displayed.
 * - $help_text: TRUE if the store help message should be displayed.
 * - $email_text: TRUE if the "do not reply to this email" message should
 *   be displayed.
 * - $store_footer: TRUE if the store URL should be displayed.
 * - $thank_you_message: TRUE if the 'thank you for your order' message
 *   should be displayed.
 *
 * @see template_preprocess_uc_order()
 */
?>
<table class="uc-order-customer" align="center">
  <tr>
    <td>
      <table class="uc-order-customer-inner" align="center">
        <?php if ($business_header): ?>
        <tr valign="top">
          <td>
            <table class="uc-order-customer-header" width="100%">
              <tr>
                <?php if ($site_logo): ?>
                  <td class="uc-order-customer-logo">
                    <div style="padding-right: 1em;">
                      <?php print $site_logo; ?>
                    </div>
                  </td>
                <?php endif; ?>
                <td class="uc-order-customer-name" width="98%">
                  <span style="font-size: large;"><?php print $store_name; ?></span><br />
                  <?php print $site_slogan; ?>
                </td>
                <td class="uc-order-customer-detail" nowrap="nowrap">
                  <?php print $store_address; ?><br /><?php print $store_phone; ?>
                </td>
              </tr>
            </table>
          </td>
        </tr>
        <?php endif; ?>

        <tr valign="top">
          <td>

            <?php if ($thank_you_message): ?>
            <div class="uc-order-customer-message">
              <p><b><?php print t('Thanks for your order, !order_first_name!', array('!order_first_name' => $order_first_name)); ?></b></p>

              <?php if (isset($order->data['new_user'])): ?>
              <p><b><?php print t('An account has been created for you with the following details:'); ?></b></p>
              <p><b><?php print t('Username:'); ?></b> <?php print $order_new_username; ?><br />
              <b><?php print t('Password:'); ?></b> <?php print $order_new_password; ?></p>
              <?php endif; ?>

              <p><b><?php print t('Want to manage your order online?'); ?></b><br />
              <?php print t('If you need to check the status of your order, please visit our home page at !store_link and click on "My account" in the menu or login with the following link:', array('!store_link' => $store_link)); ?>
              <br /><br /><?php print $site_login_link; ?></p>
            </div>
            <?php endif; ?>

            <table class="uc-order-customer-main">
              <tr>
                <td colspan="2">
                  <div class="uc-order-customer-header"><?php print t('Purchasing Information:'); ?></div>
                </td>
              </tr>
              <tr>
                <td nowrap="nowrap">
                  <label><?php print t('E-mail Address:'); ?></label>
                </td>
                <td width="98%">
                  <?php print $order_email; ?>
                </td>
              </tr>
              <tr>
                <td colspan="2">

                  <table class="uc-order-customer-addresses">
                    <tr>
                      <td valign="top" width="50%">
                        <div class="uc-order-customer-label"><?php print t('Billing Address:'); ?></div>
                        <div class="uc-order-customer-billing-address">
                          <?php print $order_billing_address; ?>
                        </div>
                        <div class="uc-order-customer-label"><?php print t('Billing Phone:'); ?></div>
                        <div class="uc-order-customer-billing-phone">
                          <?php print $order_billing_phone; ?><br />
                        </div>
                      </td>
                      <?php if ($shippable): ?>
                      <td valign="top" width="50%">
                        <div class="uc-order-customer-label"><?php print t('Shipping Address:'); ?></div>
                        <div class="uc-order-customer-shipping-address">
                          <?php print $order_shipping_address; ?>
                        </div>
                        <div class="uc-order-customer-label"><?php print t('Shipping Phone:'); ?></div>
                        <div class="uc-order-customer-shipping-phone">
                          <?php print $order_shipping_phone; ?>
                        </div>
                      </td>
                      <?php endif; ?>
                    </tr>
                  </table>

                </td>
              </tr>
              <tr>
                <td nowrap="nowrap">
                  <label><?php print t('Order Grand Total:'); ?></label>
                </td>
                <td width="98%">
                  <b><?php print $order_total; ?></b>
                </td>
              </tr>
              <tr>
                <td nowrap="nowrap">
                  <label><?php print t('Payment Method:'); ?></label>
                </td>
                <td width="98%">
                  <?php print $order_payment_method; ?>
                </td>
              </tr>

              <tr>
                <td colspan="2">
                  <div class="uc-order-customer-header"><?php print t('Order Summary:'); ?></div>
                </td>
              </tr>

              <?php if ($shippable): ?>
              <tr>
                <td>
                  <div class="uc-order-customer-shipping"><?php print t('Shipping Details:'); ?></div>
                </td>
              </tr>
              <?php endif; ?>

              <tr>
                <td colspan="2">

                  <table class="uc-order-customer-addresses">
                    <tr>
                      <td nowrap="nowrap">
                        <label><?php print t('Order #:'); ?></label>
                      </td>
                      <td width="98%">
                        <?php print $order_link; ?>
                      </td>
                    </tr>

                    <tr>
                      <td nowrap="nowrap">
                        <label><?php print t('Order Date: '); ?></label>
                      </td>
                      <td width="98%">
                        <?php print $order_created; ?>
                      </td>
                    </tr>

                    <?php if ($shipping_method && $shippable): ?>
                    <tr>
                      <td nowrap="nowrap">
                        <label><?php print t('Shipping Method:'); ?></label>
                      </td>
                      <td width="98%">
                        <?php print $order_shipping_method; ?>
                      </td>
                    </tr>
                    <?php endif; ?>

                    <tr>
                      <td nowrap="nowrap">
                        <?php print t('Products Subtotal:'); ?>&nbsp;
                      </td>
                      <td width="98%">
                        <?php print $order_subtotal; ?>
                      </td>
                    </tr>

                    <?php foreach ($line_items as $item): ?>
                    <?php if ($item['type'] == 'subtotal' || $item['type'] == 'total')  continue; ?>

                    <tr>
                      <td nowrap="nowrap">
                        <?php print $item['title']; ?>:
                      </td>
                      <td>
                        <?php print $item['formatted_amount']; ?>
                      </td>
                    </tr>

                    <?php endforeach; ?>

                    <tr>
                      <td>&nbsp;</td>
                      <td>------</td>
                    </tr>

                    <tr>
                      <td nowrap="nowrap">
                        <label><?php print t('Total for this Order:'); ?>&nbsp;</label>
                      </td>
                      <td>
                        <label><?php print $order_total; ?></label>
                      </td>
                    </tr>

                    <tr>
                      <td colspan="2">
                        <br /><br /><label><?php print t('Products on order:'); ?>&nbsp;</label>

                        <table class="uc-order-customer-products">

                          <?php foreach ($products as $product): ?>
                          <tr>
                            <td width="2%" valign="top" nowrap="nowrap">
                              <span class="uc-order-customer-qty"><?php print $product->qty; ?> x </span>
                            </td>
                            <td width="98%">
                              <div><span><?php print $product->title; ?> - <?php print $product->total_price; ?></span></div>
                              <div><?php print $product->individual_price; ?></div>
                              <div><?php print t('SKU'); ?>: <?php print $product->model; ?></div>
                              <div><?php print $product->details; ?></div>
                            </td>
                          </tr>
                          <?php endforeach; ?>
                        </table>

                      </td>
                    </tr>
                  </table>

                </td>
              </tr>

              <?php if ($help_text || $email_text || $store_footer): ?>
              <tr class="uc-order-customer-footer-wrapper">
                <td colspan="2">
                  <hr noshade="noshade" size="1" /><br />

                  <?php if ($help_text): ?>
                  <label><?php print t('Where can I get help with reviewing my order?'); ?></label>
                  <div class="uc-order-customer-help"><?php print t('To learn more about managing your orders on !store_link, please visit our <a href="!store_help_url">help page</a>.', array('!store_link' => $store_link, '!store_help_url' => $store_help_url)); ?>
                  </div>
                  <?php endif; ?>

                  <?php if ($email_text): ?>
                  <div class="uc-order-customer-email"><?php print t('Please note: This e-mail message is an automated notification. Please do not reply to this message.'); ?></div>

                  <div class="uc-order-customer-thx"><?php print t('Thanks again for shopping with us.'); ?></div>
                  <?php endif; ?>

                  <?php if ($store_footer): ?>
                  <div class="uc-order-customer-footer">
                    <div class="uc-order-customer-link"><?php print $store_link; ?></div>
                    <div class="uc-order-customer-slogan"><?php print $site_slogan; ?></div>
                  </div>
                  <?php endif; ?>
                </td>
              </tr>
              <?php endif; ?>

            </table>
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>
