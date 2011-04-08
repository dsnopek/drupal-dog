<?php

function dog_drush_help($section) {
  switch ($section) {
    case 'meta:dog:title':
      return dt('Drupal on Git (Dog) commands');
    case 'meta:dog:summary':
      return dt('Build, develop, manage, and deploy an entire Drupal site using the dog methodology.');
  }
}

function dog_drush_command() {
  $items = array();

  $items['dog-rollout'] = array(
    'description' => 'Roll out a new dog-managed Drupal instance either from an existing dog repository or a raw dog manifest.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'arguments' => array(
      'source' => 'A URI, resolving either to an already-built dog Git repository, or a raw dog manifest from which a new dog repository should be created.',
      'destination' => 'Path to the desired root of the instance. If unspecified, defaults to the `git-clone` behavior that inherits the local directory name from the remote URI; e.g., "git://...drupal.git" will be cloned into the relative path "drupal".',
    ),
    'aliases' => array('dogro'),
  );

  $items['dog-status'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'description' => 'Report on the state of the current dog-managed Drupal instance, as it pertains to dog.',
    'aliases' => array('dogs'),
  );

  $items['dog-rollup'] = array(
    'description' => 'Update the dog manifests & metadata, or init them if they have not yet been created.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'options' => array(
      'no-commit' => 'Perform the roll up, but do not automatically commit the changes. By default, dog',
    ),
    'aliases' => array('dogru'),
  );

  $items['dog-resync'] = array(
    'description' => 'Update an existing dog-managed Drupal site to the latest state recorded in the main collab repository. Any changes to the ',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'arguments' => array(
      'repositories' => 'A space-delimited list of repositories attached to the dog instance that should be synced. If none specified, all repositories are synced.',
    ),
    'options' => array(
      'wc-strategy' => 'Defines the behavior to use when working copy changes are encountered during sync. Acceptable values: stash (stash any changes found in the working copy), stash-reapply (stash, update, `git stash apply`), stash-pop (stash, update, `git stash pop`), discard (`git reset --hard HEAD`), skip (omit the repo from sync). Defaults to stash-pop.',
    ),
    'aliases' => array('dogre'),
  );

  $items['dog-revert'] = array(
    'description' => 'Roll back a dog resync operation.',
  );

  $items['dog-dl'] = array(
    'description' => 'Download contributed projects from Drupal.org via git cloning and register them into the dog system. This is essentially a wrapper on `drush dl` without the "--version-control*" and "--package-handler*" arguments, as those considerations are handled by dog.',
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'options' => array(
      'collab-uri' => 'A writeable Git remote repository URI to be registered for the "collab" remote.',
    ),
  );

  $items['dog-remove-extension'] = array(
    'description' => 'Remove a dog-managed extension from the site. This removes the attached git repository and removes all references to it from dog\'s metadata.',
  );

  $items['dog-log'] = array(
    'description' => 'Wrapper on `git log` that helps filter log output to exclude upstream and irrelevant commit activity.',
  );

  return $items;
}
