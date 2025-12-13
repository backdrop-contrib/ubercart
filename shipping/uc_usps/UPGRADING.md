# Upgrading to the new USPS API

## Overview

For many years, the Drupal 7 and Backdrop versions of Ubercart have used the USPS Web Tools API for postal rate lookups. [That API is now being deprecated by USPS](https://www.usps.com/business/web-tools-apis/welcome.htm) in favor of their [new OAuth2-based API](https://developers.usps.com). This new API makes many changes that will affect Ubercart stores that use the `uc_usps` module to do rate lookups:

* There are options that are no longer available (like "online prices");
* There are new options that are available (like a distinction between Retail, Commercial, and Contract rates).

To support Ubercart stores that use the `uc_usps` module, this module now supports both the **Legacy** API (the old Web Tools API) and the **new API**. New installations can choose which API to use. However:

* **The old Web Tools API will be turned off by USPS on January 25, 2026.**

All Ubercart stores should switch to the new API before then. Future releases of Backdrop CMS Ubercart will only support the new USPS API going forward.

## Background: the Legacy API

The Ubercart USPS module `uc_usps` recognizes two options for the **Default product shipping type**: _Envelope_ and _Small package_. Shipping type _Small package_ is a default shipping type defined by the `uc_quote` module. Module `uc_usps` adds the additional shipping type of _Envelope_.

You can then further set a package type for the product (labeled **Package type** on the product edit form); this select element has many options, such as _Variable_, _Flat rate envelope_, and more. However, only _Small package_ lets you set the package type to anything other than _Variable_.

That means that any _Envelope_ products can still be offered to be shipped in a box. You can't actually specify that it can _only_ be shipped in some type of envelope. However, this is useful, because if you have selected the _All in one_ option in the quote options for USPS, it will permit both _Envelope_ and _Small package_ products to be shipped in the same box.

Another characteristic of the existing USPS module is that while it offers the user a choice of several different rates, it will ship all packages via the same rate, even if it would be cheaper to ship, say, one product in an envelope and another in a box.

(Note that there was a bug in the rendering of of the services presented to the user to choose from, which did not correctly list the number of packages being shipped. This has been fixed in the current version.)

When you upgrade Ubercart to this version, your settings will be left using the legacy API. You will need to manually switch to the new API at a time that is convenient for you. You can switch back and forth between the two APIs in order to see the effects of changes.

To upgrade to the new API for rate lookups, you will need to select the new API in the settings for USPS shipping and then choose various configuration options, some of which are similar to the legacy API options and some of which are new. We strongly recommend doing this at a time where there are no carts in checkout or other pending statuses, as the results of a new lookup versus an old lookup may well be different in price, type of mail, et cetera. We suggest taking the store offline, then changing the configuration settings, and, if possible, carrying out several test orders to verify that all lookups are working well before bringing the store back online.

## Setting up OAuth2 Credentials with USPS

The new USPS API uses OAuth2 validation, which you will need to set up on the [USPS Developer Portal](https://developers.usps.com). In the old API, you had a USPS User ID, which never changed and was all that was needed to do a rate lookup. With the OAuth2 API, every lookup contains an OAuth2 access token, which is time-limited and expires after several hours. The USPS module takes care of obtaining the necessary token and renewing it regularly; this is done transparently, so you don't need to worry about it. However, in order to obtain the token, you will need to obtain a Consumer Key and a Consumer Secret (which are both long strings, the latter longer than the former), and enter these into the settings.

You will do that by creating an account on the USPS site (click "Sign up" on the Developer Portal), but you may already have an account (you needed one to create the original User ID for the Web Tools API).

Once you are signed in, go to developers.usps.com, and you will see Apps in the top menu bar. Click on that—you will need to create an "App" for API access. Click on the "Add App" button, which will open up a form:

* App name — give your App a name (which could be the name of your store).
* Callback URL — This is not used by `uc_usps`, so you can leave it blank.
* Description — make this whatever you want.
* APIs — Check the "Public Access I" box (the only one). This is what will give you access to the rate lookup endpoints (and several others not used by `uc_usps`, though they might be added in the future).

Then click "Add App." This will create the new App.

When the App is created, you will see some details about it. Two are very important:

* Consumer Key — this is the "Consumer Key" that you will need to enter into Ubercart settings.
* Consumer Secret — this is the "Consumer Secret" that you will need to enter into Ubercart settings.

Both of these are required for Ubercart to obtain an access token in order to do rate lookups. Copy them both, and keep them in a safe place (ideally, an encrypted safe place).

### Configure Ubercart USPS

Now you are ready to configure the USPS shipping settings.

## Configuration

The configuration options can be found at Admin > Store > Configuration > Shipping quotes > Settings > USPS, i.e., at path admin/store/settings/quotes/settings/usps. Click through each of the vertical tab sets as described below, then click "Save Configuration" to switch to the new API. Your legacy API settings will remain saved, so you can easily switch back and forth.

### Credentials

The first setting in the Credentials tab is **Method**: select _New API_ to switch to the new API. When you do this, the fields below **Method** will change. (New installations of Ubercart will automatically be set to use the new API.)

#### USPS Consumer Key

Enter the "Consumer Key" from your USPS account.

#### USPS Consumer Secret

Enter the "Consumer Secret" from your USPS account. This will be saved but not displayed when you save the configuration; you shouldn't need to enter it again.

#### Connection address

Select either "Testing" or "Production". Use the former while you are testing out the system, the latter once you are processing real transactions.

Once you have entered those values, click "Save Configuration." You should see a success message posted to the top of the stage and the **Access token** field will be populated. This token expires after 8 hours but will be automatically updated as needed.

### USPS domestic

This tab lists all of the services for _Envelope_ and _Small package_ products. These are generally different from the services offered by the legacy API, although there is a rough correspondence. For example, "First Class Mail Stamped Letter" in the legacy API is "First Class Letter Metered" in the new API. Select the services that you wish to offer for each type of package; you should find rough equivalents to the legacy API settings in the new services lists.

### USPS international

The same goes for international services.

### Quote options

#### Product packages

As in the legacy API, you can choose whether each product should be shipped in its own package or all products shipped in a single package. Note that with multiple packages, all packages will be shipped via the same service, so you should not choose service options for _Envelope_ and _Small package_ that are incompatible if there is a possibility of both products being shipped together.

#### Price type

This is a new setting, which replaced the previous "Use online rates" checkbox. You can select Retail, Commercial, or Contract rates to quote. (Retail rates are often a bit higher than Commercial rates.)

#### Markups

Markups are the same as they were in the legacy API: you can mark up products by a percentage of their price and/or mark up the weight sent to the rate lookup to account for the weight of shipping materials, etc.

When you are done, click "Save Configuration." You're done! You and your customers can now continue to put items in the cart and checkout as usual. During checkout, customers will be presented with a choice of all qualifying services and their rates, so do make sure that you have only allowed those services that you are prepared to provide.

## Manual USPS Rate Lookups

The uc_usps module provides the ability to perform a manual lookup via the USPS OAuth2 API, where you can provide a package's size, weight, and destination, and see what rates are returned by the USPS. This is  separate from Ubercart's lookups for Ubercart products and packages, but it might be helpful to store administrators in deciding what types of rates to select in the store configuration. There are documentation links on the form for the relevant USPS APIs and lookup types.

To perform a manual USPS rate lookup, you need to have fully configured the New API as described above (i.e., obtained and entered a valid Consumer Key and Consumer Secret). You also need to have enabled the UC Reports module (uc_reports). The lookup form may be found at Admin > Store > Reports > USPS Lookup. Once on that form, you can choose the lookup type, relevant package parameters, and what types of results to see for each lookup. If you have the [Devel module](https://backdropcms.org/project/devel) installed and enabled, you can choose to see some of the results information in click-expandable (krumo) form.

Note that the results are purely those returned by the USPS for the given set of parameters. If you provide inconsistent parameters, the result will be the error code(s) returned from USPS.
