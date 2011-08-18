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
 * Drush command callback to create a new dog instance.
 */
function drush_dog_init($target = '') {
  // Initialize the RepositoryConfig object we'll use to control the new repo.
  $config = new Dog\Config\RepositoryConfig();

  $config['git']['remote.upstream.url'] = drush_get_option('upstream', 'drupal');

  // Force inclusion of download.pm.inc. Ugh.
  drush_include(DRUSH_BASE_PATH . '/commands/pm', 'download.pm');

  if (strpos($config['git']['remote.upstream.url'], 'drupal') === 0) {
    $version = pm_parse_project_version(array($config['git']['remote.upstream.url']));
    $version = reset($version);
    $xml = _drush_pm_get_release_history_xml($version);
    $release = _pm_download_parse_release($version, $xml);
    $config['upstream_ref'] = drush_get_option('upstream-ref', $release['tag']);
    $config['git']['remote.upstream.url'] = 'git://git.drupal.org/project/drupal.git';
  }
  else {
    $config['upstream_ref'] = drush_get_option('upstream-ref');
    if (!is_string($config['upstream_ref'])) {
      return drush_set_error('DRUSH_DOG_INVALID_UPSTREAM', dt('An upstream ref must be specified as the starting point for the dog instance.'));
    }
  }

  $config['git']['remote.collab.url'] = drush_get_option('collab', $config['git']['remote.upstream.url']);

  $target = $target ?: preg_replace('/.*\/([\w]*)(\.git)?$/', '\1', $config['git']['remote.upstream.url']);

  // Normalize target to absolute path
  $realtarget = $target[0] != '/' ? getcwd() . '/' . $target : $target;
  $config['worktree'] = $realtarget;
  $config['repopath'] = "$realtarget/.git";

  if (file_exists($realtarget)) {
    return drush_set_error('DRUSH_DOG_TARGET_DIRECTORY_EXISTS', dt("Target directory '$realtarget' already exists."));
  }

  // Pop out a new face
  $face = new Dog\Face($realtarget, TRUE);
  $core = new Dog\Repository\Core($config, $face);
  $core->create();

  // Initialize the sledfile
  mkdir($config['worktree'] . '/.dog');
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
