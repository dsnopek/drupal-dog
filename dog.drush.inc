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

  $items['dog-init'] = array(
    'description' => 'Set up a new dog-managed Drupal instance.',
    'arguments' => array(
      'target-dir' => "The target directory where the instance should be initialized. Defaults to the upstream repository name if unspecified (same behavior as `git clone`).",
    ),
    'options' => array(
      'upstream' => 'The Drupal core repository to use as the upstream variant of Drupal. Accepts either a full URI or drush dl style input (e.g. "drupal-7.x"). If unspecified, defaults to vanilla core (git://git.drupal.org/project/drupal.git).',
      'branch' => "The name of the local branch to create for collaboration. Defaults to 'master'.",
      'collab' => "The URI to use as the collab repository for this instance. If unspecified, the upstream URI is used.",
    ),
    'examples' => array(
      'drush dog-init --upstream=6.x --collab=git://github.com/me/my-project my_project' =>
        'Initialize a new Drupal 6.x project in a directory called "my_project" with the upstream from git.drupal.org and a collab repository on github.',
    ),
  );

  $items['dog-dl'] = array(
    'description' => 'Download contributed projects from Drupal.org via git cloning and register them into the dog system. This is essentially a wrapper on `drush dl` without the "--version-control*" and "--package-handler*" arguments, as those considerations are handled by dog.',
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'arguments' => array(
      'project' => 'The contrib project from drupal in drush dl style (e.g "views-6.2.x"). Also accepts a full git URI for using non-drupal-hosted projects.',
      'target-dir' => 'The target directory where the contrib should be initialized. Defaults to sites/all/<contrib-type>/.',
    ),
    'options' => array(
      'collab' => 'A writeable Git remote repository URI to be registered for the "collab" remote.',
    ),
    'examples' => array(
      'drush dog-dl views-6.2.x' =>
        'Initialize views 6.2 in the project repository in the default location',
      'drush dog-dl https://github.com/kete/tiny_mce.git sites/all/libraries/tiny_mce' =>
        'Initialize tiny_mce from github in sits/all/libraries',
    ),

  );
  
  $items['dog-collab'] = array(
    'description' => 'Changes the collab remote to a module, theme or main repository. Useful if one was not set up initially or flexibility is required.',
    'arguments' => array(
      'collab-uri' => 'URI location for the collab remote git repository',
      'target-dir' => 'Path to the module or theme directory, or drupal root where the collab should be added. Defaults to current working directory.',
    ),
    'examples' => array(
      'drush dog-collab git://github.com/me/my-views sites/all/modules/views' =>
        'Add a collab repo from github to the views module',
    ),
  );

  $items['dog-catchup'] = array(
    'description' => 'Update an existing dog-managed Drupal site to the latest state recorded in the main collab repository.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'arguments' => array(
      'repositories' => 'A space-delimited list of repositories attached to the dog instance that should be synced. If none specified, all repositories are synced.',
    ),
    'options' => array(
      'wc-strategy' => 'Defines the behavior to use when working copy changes are encountered during sync. Acceptable values: stash (stash any changes found in the working copy), stash-reapply (stash, update, `git stash apply`), stash-pop (stash, update, `git stash pop`), discard (`git reset --hard HEAD`), skip (omit the repo from sync). Defaults to stash-pop.',
    ),
    'aliases' => array('dogre'),
  );
  
  $items['dog-upstream-update'] = array(
    'description' => 'Pull updates from upstream and commit them to the collab remote if one exists.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'arguments' => array(
      'target-dir' => 'Path to the module or theme directory, or drupal root where the collab should be added. Defaults to current working directory',
    ),
    'aliases' => array('dogup'),
  );
  
  $items['dog-status'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'description' => 'Report on the state of the current dog-managed Drupal instance, as it pertains to dog.',
    'aliases' => array('dogs', 'vet'),
  );

  $items['dog-remove'] = array(
    'description' => 'Remove a dog-managed extension from the site. This removes the attached git repository and removes all references to it from dog\'s metadata.',
    'arguments' => array(
      'target-dir' => 'Path to the module or theme directory, or drupal root where the collab should be added.',
    ),
    'aliases' => array('doggone'),
  );

  $items['dog-rollout'] = array(
    'description' => 'Roll out a new dog-managed Drupal instance either from an existing dog repository or a raw dog manifest.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'arguments' => array(
      'source' => 'A URI, resolving either to an already-built dog Git repository, or a raw dog manifest from which a new dog repository should be created.',
      'destination' => 'Path to the desired root of the instance. If unspecified, defaults to the `git-clone` behavior that inherits the local directory name from the remote URI; e.g., "git://...drupal.git" will be cloned into the relative path "drupal".',
    ),
    'aliases' => array('dogro'),
  );

  $items['dog-rollup'] = array(
    'description' => 'Update the dog manifests & metadata, or init them if they have not yet been created.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'options' => array(
      'no-commit' => 'Perform the roll up, but do not automatically commit the changes. By default, dog',
    ),
    'aliases' => array('dogru'),
  );

  $items['dog-revert'] = array(
    'description' => 'Roll back a dog resync operation.',
  );

  $items['dog-log'] = array(
    'description' => 'Wrapper on `git log` that helps filter log output to exclude upstream and irrelevant commit activity.',
  );

  return $items;
}
