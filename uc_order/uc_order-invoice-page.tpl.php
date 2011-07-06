<?php

/**
 * @file
 * Default theme implementation to display a printable Ubercart invoice.
 *
 * @see template_preprocess_uc_order_invoice_page()
 */
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">

<head>
  <title><?php print $head_title; ?></title>
</head>
<body>
  <div align="right" style="margin-bottom: 1em; margin-right: 1em;">
    <input type="button" value="<?php print t('Print invoice'); ?>" onclick="window.print();" />
    <input type="button" value="<?php print t('Close window'); ?>" onclick="window.close();" />
  </div>

  <?php print $content; ?>
</body>
</html>
