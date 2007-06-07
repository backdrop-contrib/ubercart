<?php
// $Id$

/**
 * This file is the default admin notification template for Ubercart.
 */
?>

<p>
<?php echo t('Order number:'); ?> [order-admin-link]<br />
<?php echo t('Customer:'); ?> [order-first-name] [order-last-name] - [order-email]<br />
<?php echo t('Order total:'); ?> [order-total]<br />
<?php echo t('Shipping method:'); ?> [order-shipping-method]
</p>

<p>
<?php echo t('Products:'); ?><br />
<?php foreach ($products as $product) { ?>
- <?php echo $product->qty; ?> x <?php echo $product->title .' - '. uc_currency_format($product->price * $product->qty); ?><br />
&nbsp;&nbsp;<?php echo t('Model: ') . $product->model; ?><br />
    <?php if (is_array($product->data['attributes']) && count($product->data['attributes']) > 0) {?>
    <?php foreach ($product->data['attributes'] as $key => $value) {
      echo '&nbsp;&nbsp;'. $key .': '. $value .'<br />';
    } ?>
    <?php } ?>
<br />
<?php } ?>
</p>

<p>
<?php echo t('Order comments:'); ?><br />
[order-comments]
</p>
