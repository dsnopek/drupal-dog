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
  if (!drush_get_context('DOG', FALSE)) {
    return drush_set_error('DRUSH_DOG_NOT_DOG_INSTANCE');
  }

  // Check git_deploy is enabled. Only for bootstrapped sites.
  if (drush_get_context('DRUSH_BOOTSTRAP_PHASE') >= DRUSH_BOOTSTRAP_DRUPAL_FULL) {
    if (!module_exists('git_deploy')) {
      drush_log(dt('git package handler needs git_deploy module enabled to work properly.'), 'warning');
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
  // Uses HTTP, ensuring maximum (port) accessibility
  $repository = 'http://git.drupal.org/project/' . $request['name'] . '.git';
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
  // FIXME this is all still code copied over from git_drupalorg
  drush_log('Updating project ' . $request['name'] . ' ...');

  $commands = array();
  if ($release['version_extra'] == 'dev') {
    // Update the branch of the development repository.
    $commands[] = 'git pull';
    $commands[] = drush_get_option('gitpullparams');
  }
  else {
    // Use a stable repository.
    $commands[] = 'git fetch';
    $commands[] = drush_get_option('gitfetchparams');
    $commands[] = ';';
    $commands[] = 'git checkout';
    $commands[] = drush_get_option('gitcheckoutparams');
    $commands[] = $release['version'];
  }

  if (!drush_shell_cd_and_exec($request['full_project_path'], implode(' ', $commands))) {
    return drush_set_error('DRUSH_PM_UNABLE_CHECKOUT', 'Unable to update ' . $request['name'] . ' from git.drupal.org.');
  }

  return TRUE;
}

/**
 * Post download action, which is triggered after the extension code has been
 * moved into its final location.
 *
 * Here we add the new repository as a submodule and register its data into the
 * sledfile.
 */
function package_handler_post_download($project) {
  // FIXME this is all still code copied over from git_drupalorg
  if (drush_get_option('gitsubmodule', FALSE)) {
    // Obtain the superproject path, then add as submodule.
    if (drush_shell_cd_and_exec(dirname($project['full_project_path']), 'git rev-parse --show-toplevel')) {
      $output = drush_shell_exec_output();
      $superproject = $output[0];
      // Add the downloaded project as a submodule of its git superproject.
      $command = array();
      $command[] = 'git submodule add';
      $command[] = drush_get_option('gitsubmoduleaddparams');
      $command[] = $project['repository'];
      // We need the submodule relative path.
      $command[] = substr($project['full_project_path'], strlen($superproject) + 1);
      if (!drush_shell_cd_and_exec($superproject, implode(' ', $command))) {
        return drush_set_error('DRUSH_PM_GIT_CHECKOUT_PROBLEMS', dt('Unable to add !name as a git submodule of !super.', array('!name' => $project['name'], '!super' => $superproject)));
      }
    }
    else {
      return drush_set_error('DRUSH_PM_GIT_SUBMODULE_PROBLEMS', dt('Unable to create !project as a git submodule: !dir is not in a Git repository.', array('!project' => $project['name'], '!dir' => dirname($project['full_project_path']))));
    }
  }
}
