<?php

function drush_gsb_drush_command() {
  $items = array();

  $items['gsb-status'] = array(
    'description' => 'Report the status of the current GSB-managed Drupal site. If the site does not appear to be a valid GSB instance, indicate as such.',
  );

  $items['gsb-dl'] = array(
    'description' => 'Download contributed projects from Drupal.org via git cloning and register them into the GSB system. This is essentially a wrapper on `drush dl` without the "--version-control*" and "--package-handler*" arguments, as those considerations are handled by GSB.',
  );

  $items['gsb-remove'] = array(
    'description' => 'Remove a GSB-managed project from the site. This removes the attached git repository and removes all references to it from GSB\'s metadata.',
  );

  $items['gsb-create'] = array(
    'description' => 'Set up a new GSB-managed Drupal site. Can be built from one of a variety of sources.',
  );

  $items['gsb-update'] = array(
    'description' => 'Update the GSB manifests & metadata, or init them if they have not yet been created.',
  );

  $items['gsb-resync'] = array(
    'description' => 'Safely update an existing GSB-managed Drupal site to the latest state recorded in the main collab repository.',
  );

  $items['gsb-rollback'] = array(
    'description' => 'Roll back a GSB resync operation.',
  );

  $items['gsb-audit'] = array(
    'description' => 'Perform a thorough review on an entire GSB-managed Drupal site and report any problems.',
  );

  return $items;
}
