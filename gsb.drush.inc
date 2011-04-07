<?php

function gsb_drush_help($section) {
  switch ($section) {
    case 'meta:gsb:title':
      return dt('Git Site Builder commands');
    case 'meta:gsb:summary':
      return dt('Build, develop, manage, and deploy an entire Drupal site using the GSB methodology.');
  }
}

function gsb_drush_command() {
  $items = array();

  $items['gsb-rollout'] = array(
    'description' => 'Roll out a new GSB-managed Drupal instance either from an existing GSB repository or a raw GSB manifest.',
    'arguments' => array(
      'source' => 'A URI, resolving either to an already-built GSB Git repository, or a raw GSB manifest from which a new GSB repository should be manifest.',
      'destination' => 'Path to the desired root of the instance. If unspecified, defaults to the `git-clone` behavior that inherits the local directory name from the remote URI; e.g., "git://...drupal.git" will be cloned into the relative path "drupal".',
    ),
  );

  $items['gsb-status'] = array(
    'description' => 'Report the status of the current GSB-managed Drupal site. If the site does not appear to be a valid GSB instance, indicate as such.',
  );

  $items['gsb-rollup'] = array(
    'description' => 'Update the GSB manifests & metadata, or init them if they have not yet been created.',
  );

  $items['gsb-resync'] = array(
    'description' => 'Safely update an existing GSB-managed Drupal site to the latest state recorded in the main collab repository.',
  );

  $items['gsb-rollback'] = array(
    'description' => 'Roll back a GSB resync operation.',
  );

  $items['gsb-dl'] = array(
    'description' => 'Download contributed projects from Drupal.org via git cloning and register them into the GSB system. This is essentially a wrapper on `drush dl` without the "--version-control*" and "--package-handler*" arguments, as those considerations are handled by GSB.',
  );

  $items['gsb-remove'] = array(
    'description' => 'Remove a GSB-managed project from the site. This removes the attached git repository and removes all references to it from GSB\'s metadata.',
  );

  $items['gsb-log'] = array(
    'description' => 'Wrapper on `git log` that helps filter log output to exclude upstream and irrelevant commit activity.',
  );

  return $items;
}
