<?php
// $Id$

/**
 * @file
 * This file is the default admin notification template for Ubercart.
 */
?>

<p>
<?php echo t('Order number:'); ?> [uc_order:admin-link]<br />
<?php echo t('Customer:'); ?> [uc_order:first-name] [uc_order:last-name] - [uc_order:email]<br />
<?php echo t('Order total:'); ?> [uc_order:total]<br />
<?php echo t('Shipping method:'); ?> [uc_order:shipping-method]
</p>

<p>
<?php echo t('Products:'); ?><br />
<?php
$context = array(
  'revision' => 'themed',
  'type' => 'order_product',
);
foreach ($products as $product) {
  $price_info = array(
    'price' => $product->price,
    'qty' => $product->qty,
  );
  $context['subject'] = array(
    'order_product' => $product,
  );
?>
- <?php echo $product->qty; ?> x <?php echo $product->title . ' - ' . uc_price($price_info, $context); ?><br />
&nbsp;&nbsp;<?php echo t('SKU: ') . $product->model; ?><br />
    <?php if (is_array($product->data['attributes']) && count($product->data['attributes']) > 0) {?>
    <?php foreach ($product->data['attributes'] as $attribute => $option) {
      echo '&nbsp;&nbsp;' . t('@attribute: @options', array('@attribute' => $attribute, '@options' => implode(', ', (array)$option))) . '<br />';
    } ?>
    <?php } ?>
<br />
<?php } ?>
</p>

<p>
<?php echo t('Order comments:'); ?><br />
[uc_order:comments]
</p>
