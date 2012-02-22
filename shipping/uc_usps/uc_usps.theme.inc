<?php

/**
 * @file
 * Theme functions for the uc_usps module.
 */

/**
 * Theme function to format the USPS service name and rate amount line-item
 * shown to the customer.
 *
 * @param $variables
 *   Associative array containing information needed to theme a quote.
 *   Contains two keys:
 *   - service: The USPS service name.
 *   - packages: Package information.
 *
 * @ingroup themeable
 */
function theme_uc_usps_option_label($variables) {
  $service = $variables['service'];
  $packages = $variables['packages'];

  // Start with USPS logo.
  $output  = theme('image', array(
    'path' => drupal_get_path('module', 'uc_usps') . '/uc_usps_logo.gif',
    'alt' => t('USPS logo'),
    'attributes' => array('class' => 'usps-logo')
  ));

  // Add USPS service name, removing the first nine characters
  // (== 'U.S.P.S. ') because these replicate the logo image
  $output .= substr($service, 9);

  // Add package information
  $output .= ' (' . format_plural(count($packages), '1 package', '@count packages') . ')';

  return $output;
}
