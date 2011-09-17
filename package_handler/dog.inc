<?php

/**
 * @file
 *
 * Dog-specific package handler.
 */

/**
 * Validate the dog package handler can run. This requires the following:
 *
 *  - The git binary can be found, either on the PATH or in a specified env var.
 *  - The version of git, if found, is at least 1.7.
 *  - We are in a valid dog-managed instance.
 */
function package_handler_validate() {
  if (!dog_get_face()) {
    drush_set_error('DRUSH_DOG_NOT_A_DOG_INSTANCE', "Not in a valid dog instance, cannot use the dog package handler..");
  }

  // Check git_deploy is enabled. Only for bootstrapped sites.
  if (drush_get_context('DRUSH_BOOTSTRAP_PHASE') >= DRUSH_BOOTSTRAP_DRUPAL_FULL) {
    if (!module_exists('git_deploy')) {
      drush_log(dt('dog package handler needs git_deploy module enabled to work properly.'), 'warning');
    }
  }
}

/**
 * Download a project.
 *
 * @param $request
 *   The project array with name, base and full (final) paths.
 * @param $release
 *   The release details array from drupal.org.
 */
function package_handler_download_project(&$request, $release) {
  $dog = &dog_get_face();
  // Uses HTTP, ensuring maximum (port) accessibility
  $repository = 'git://git.drupal.org/project/' . $request['name'] . '.git';
  $request['repository'] = $repository;

  // Need to separate the location of the cache from whether to use (and populate) it.
  // Really, this needs solving in drush proper.
  $cache = drush_get_option('git-reference-cache', drush_server_home() . '/.drush/gitcache.git');

  if (file_exists($cache)) {
    $use_cache = TRUE;
  }

  // Clone the repo into its appropriate target location.
  $command  = 'clone';
  if ($use_cache) {
    $command .= ' --reference ' . drush_escapeshellarg($cache);
  }
  if (drush_get_context('DRUSH_VERBOSE')) {
    $command .= ' --verbose --progress';
  }

  $command .= ' -o upstream';
  $command .= ' ' . drush_escapeshellarg($repository);
  $command .= ' ' . drush_escapeshellarg($request['full_project_path']);

  try {
    _dog_git_invoke($command);
  }
  catch (Exception $e) {
    return drush_set_error('DRUSH_DOG_GIT_CLONE_FAILURE', dt('Unable to clone project !name from git.drupal.org.', array('!name' => $request['name'])));
  }

  // Check if the 'tag' from the release feed is a tag or a branch.
  // If the tag exists, git will return it
  $label = $release['tag'];
  $old = substr(trim(_dog_git_invoke('symbolic-ref HEAD', $request['full_project_path'])), 11);

//  try {
//    // FIXME we should use & trust the release xml info for this
//    $is_tag = (bool) _dog_git_invoke('tag -l ' . drush_escapeshellarg($label), $request['full_project_path']);
//  }
//  catch (Exception $e) {
//    return drush_set_error('DRUSH_DOG_GIT_CLONE_FAILURE', dt('Clone of project !name from git.drupal.org failed to generate files at expected target location.', array('!name' => $request['name'])));
//  }

  // get a random name to evade namespace conflicts
  $rand = mt_rand(100000, 500000);

  // TODO juggle the combination of 1) desired label 2) default branch defined
  // upstream and 3) new branch name to create

  // TODO Hardcode to master for now; use sled settings eventually
  _dog_git_invoke('checkout -b ' . $rand . ' ' . drush_escapeshellarg($label), $request['full_project_path']);
  _dog_git_invoke('branch -D ' . $old, $request['full_project_path']);
  _dog_git_invoke('branch -M ' . $rand . ' ' . 'master', $request['full_project_path']);

  return TRUE;
}

/**
 * Update a d.o-sourced project.
 *
 * @param $request
 *   The project array with name, base and full (final) paths.
 * @param $release
 *   The release details array from drupal.org.
 */
function package_handler_update_project($request, $release) {
  $dog = &dog_get_face();
  // FIXME this needs to actually get implemented, once we're on to the dog updating commands
}

/**
 * Post download action, which is triggered after the extension code has been
 * moved into its final location.
 *
 * Here we add the new repository as a submodule and register its data into the
 * sledfile.
 */
function package_handler_post_download($project) {
  $dog = &dog_get_face();
  // Obtain the superproject path, then add as submodule.
  $parent_path = trim(_dog_git_invoke('rev-parse --show-toplevel', dirname($project['full_project_path'])));
  $cmd = 'submodule add ';
  $cmd .= $project['repository'];
  $cmd .= ' ' . substr($project['full_project_path'], strlen($parent_path) + 1);
  try {
    _dog_git_invoke($cmd, $parent_path);
  }
  catch (Exception $e) {
    return drush_set_error('DRUSH_DOG_SUBMODULE_FAILURE', dt('Unable to add !name as a git submodule of !super.', array('!name' => $project['name'], '!super' => $parent_path)));
  }
}
