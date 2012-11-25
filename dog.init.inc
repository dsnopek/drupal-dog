<?php

/**
 * @file dog.init.inc
 *
 * Commands specific to the generally out-of-band process that is init
 * operations - init and rollout.
 */

/**
 * Autoloader for initialization (init & rollout) dog commands.
 *
 */
function dog_autoload_init($class) {
  static $base_path;

  // Only take action if searching for a dog class.
  if (strpos($class, 'Dog\\') === 0) {
    if (is_null($base_path)) {
      $command = drush_get_command();
      $base_path = $command['path'] . '/lib/';
    }

    $file = $base_path . strtr($class, '\\', '/') . '.php';
    if (is_file($file)) {
      require $file;
      return TRUE;
    }
  }
}

/**
 * Implementation of drush_hook_COMMAND_validate().
 */
function drush_dog_init_validate($target = '') {
  // First check for interactive mode.
  $interactive = !drush_get_option('y', FALSE);

  // Check remote upstream url.
  $upstream = drush_get_option('upstream');
  if (empty($upstream)) {
    if ($interactive) {
      drush_print('Please select the remote upstream url');
      $upstream = drush_prompt('upstream', 'drupal', TRUE);
    }
    else {
      $upstream = 'drupal';
    }

    // ToDo: Need to add proper validation to upstream URLs.
    drush_set_option('upstream', $upstream);
  }

  // Check remote upstream ref.
  $upstream_ref = drush_get_option('upstream-ref');
  if (empty($upstream_ref)) {
    if (strpos($upstream, 'drupal') === 0) {
      drush_include_engine('release_info', 'updatexml');
      $version = pm_parse_project_version(array($upstream));
      $version = reset($version);
      $release = release_info_fetch($version);
      $upstream_ref = $release['tag'];
      // We need to set the upstream to the proper drupal git url.
      $upstream = 'git://git.drupal.org/project/drupal.git';
      drush_set_option('upstream', $upstream);
    }
    if ($interactive) {
      drush_print('Please select the remote upstream ref');
      $upstream_ref = drush_prompt('upstream-ref', $upstream_ref, TRUE);
    }

    // ToDo: Need to add proper validation to upstream refs.
    drush_set_option('upstream-ref', $upstream_ref);
  }

  // Check and normalize target path
  $target = $target ? : preg_replace('/.*\/([\w]*)(\.git)?$/', '\1', $upstream);
  // Normalize target to absolute path
  $target = $target[0] != '/' ? getcwd() . '/' . $target : $target;
  if (file_exists($target)) {
    //return drush_set_error('DRUSH_DOG_TARGET_DIRECTORY_EXISTS', dt("Target directory '$target' already exists."));
  }
  drush_set_option('target', $target);

  // Check remote collab url
  $collab = drush_get_option('collab');
  if (empty($collab)) {
    if ($interactive) {
      drush_print('Please enter the collab remote url');
      $collab = drush_prompt('collab', 'upstream', TRUE);
    }
    else {
      $collab = $upstream;
    }
  }
  if ($collab == 'upstream') {
    $collab = $upstream;
  }
  // ToDo: Need to add proper validation to upstream refs.
  drush_set_option('collab', $collab);

  // Check branch name
  $branch = drush_get_option('branch');
  if (empty($branch)) {
    if ($interactive) {
      drush_print(dt('Please enter the local branch'));
      $branch = drush_prompt('branch', 'master', TRUE);
    }
    else {
      $branch = 'master';
    }
  }
  // ToDo: Need to add proper validation to upstream refs.
  drush_set_option('branch', $branch);

  // Check db-url is set correctly
//  $db_url = drush_get_option('db-url');
//  if (empty($db_url)) {
//    if ($interactive) {
//      drush_print(dt('Please enter a valid database connection url'));
//      drush_set_option('db-url', drush_prompt('db-url', '', TRUE));
//    }
//    else {
//      drush_set_error(dt('Could not determine database connection parameters. Pass --db-url option.'));
//      return;
//    }
//  }
}

/**
 * Drush command callback to create a new dog instance.
 */
function drush_dog_init($target = '') {
  // Initialize the RepositoryConfig object we'll use to control the new repo.
  $config = new Dog\Config\RepositoryConfig();

  // Set all of the config options.
  $config['git']['remote.upstream.url'] = drush_get_option('upstream');
  $config['upstream_ref'] = drush_get_option('upstream-ref');
  $config['git']['remote.collab.url'] = drush_get_option('collab');
  $config['localbranch'] = drush_get_option('branch');
  $config['worktree'] = drush_get_option('target');
  $config['repopath'] = $config['worktree'] . "/.git";

  // Force inclusion of download.pm.inc. Ugh.
  drush_include(DRUSH_BASE_PATH . '/commands/pm', 'download.pm');

  // Pop out a new face
  $face = new Dog\Face($config['worktree'], TRUE);
  $core = new Dog\Repository\Core($config, $face);
  $core->create();

  // Initialize the sledfile
  $xml = new XmlWriter();
  $xml->openUri($config['worktree'] . '/.dog/sled.xml');
  $xml->setIndent(TRUE);
  $xml->setIndentString('  ');
  $xml->startDocument('1.0', 'UTF-8');
  $xml->writeElement('dogsled');
  $xml->flush();

  // Leave init mode, now that we've got something to write/check to.
  $face->leaveInitMode();
  $face->attachNewRepository($core);
}
