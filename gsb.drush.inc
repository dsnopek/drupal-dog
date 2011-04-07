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
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'arguments' => array(
      'source' => 'A URI, resolving either to an already-built GSB Git repository, or a raw GSB manifest from which a new GSB repository should be manifest.',
      'destination' => 'Path to the desired root of the instance. If unspecified, defaults to the `git-clone` behavior that inherits the local directory name from the remote URI; e.g., "git://...drupal.git" will be cloned into the relative path "drupal".',
    ),
    'aliases' => array('gsbro'),
  );

  $items['gsb-status'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'description' => 'Report on the state of the current GSB-managed Drupal instance, as it pertains to GSB.',
    'aliases' => array('gsbs'),
  );

  $items['gsb-rollup'] = array(
    'description' => 'Update the GSB manifests & metadata, or init them if they have not yet been created.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'options' => array(
      'no-commit' => 'Perform the roll up, but do not automatically commit the changes. By default, GSB',
    ),
    'aliases' => array('gsbru'),
  );

  $items['gsb-resync'] = array(
    'description' => 'Update an existing GSB-managed Drupal site to the latest state recorded in the main collab repository. Any changes to the ',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'arguments' => array(
      'repositories' => 'A space-delimited list of repositories attached to the GSB instance that should be synced. If none specified, all repositories are synced.',
    ),
    'options' => array(
      'wc-strategy' => 'Defines the behavior to use when working copy changes are encountered during sync. Acceptable values: stash (stash any changes found in the working copy), stash-reapply (stash, update, `git stash apply`), stash-pop (stash, update, `git stash pop`), discard (`git reset --hard HEAD`), skip (omit the repo from sync). Defaults to stash-pop.',
    ),
    'aliases' => array('gsbre'),
  );

  $items['gsb-revert'] = array(
    'description' => 'Roll back a GSB resync operation.',
  );

  $items['gsb-dl'] = array(
    'description' => 'Download contributed projects from Drupal.org via git cloning and register them into the GSB system. This is essentially a wrapper on `drush dl` without the "--version-control*" and "--package-handler*" arguments, as those considerations are handled by GSB.',
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'options' => array(
      'collab-uri' => 'A writeable Git remote repository URI to be registered for the "collab" remote.',
    ),
  );

  $items['gsb-remove'] = array(
    'description' => 'Remove a GSB-managed project from the site. This removes the attached git repository and removes all references to it from GSB\'s metadata.',
  );

  $items['gsb-log'] = array(
    'description' => 'Wrapper on `git log` that helps filter log output to exclude upstream and irrelevant commit activity.',
  );

  return $items;
}
