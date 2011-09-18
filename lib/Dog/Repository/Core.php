<?php

namespace Dog\Repository;

use Dog\Exception\GitRuntimeException;

/**
 * The repository representing the core clone that sits at the heart of every
 * Dog instance.
 *
 */
class Core extends Base {
  public function create() {
    // Let the base class do its shared prep work.
    parent::create();

    try {
      $remoteinfo = $this->gitPassthru('ls-remote ' . drush_escapeshellarg($this->config['git']['remote.upstream.url']) . ' 7.0');
    }
    catch (GitRuntimeException $e) {
      return drush_set_error('DRUSH_DOG_INVALID_UPSTREAM', dt("Upstream URI '%upstream' is not a Git repository.", array('%upstream' => $this->config['git']['remote.upstream.url'])));
    }

    // Check that the 7.0 tag is the SHA1 we know it should be.
    if (strpos($remoteinfo, '497914920385b7016ac9c9367e0198530787adf2') !== 0) {
      return drush_set_error('DRUSH_DOG_INVALID_UPSTREAM', dt("Upstream URI '%upstream' is not a valid Drupal core repository.", array('%upstream' => $this->config['git']['remote.upstream.url'])));
    }

    // Build the base clone command.
    $cmd = 'clone -q -o upstream --no-hardlinks ';

    // Use a refcache if drush has one specified
    if ($refcache = drush_get_option('git-reference-cache')) {
      $cmd .= "--reference $refcache ";
    }

    // add the clone uri and target dir
    $cmd .= drush_escapeshellarg($this->config['git']['remote.upstream.url']);

    // add the target output dir
    $cmd .= ' ' . drush_escapeshellarg($this->config['worktree']);

    drush_log(dt("Cloning '%upstream' into %realtarget.", array('%upstream' => $this->config['git']['remote.upstream.url'], '%realtarget' => $this->config['worktree'])), 'info');

    try {
      // Do the clone
      $this->gitPassthru($cmd);
    }
    catch (Exception $e) {
      return drush_set_error('DRUSH_DOG_GIT_INVOCATION_ERROR', dt("Clone of upstream repository failed."));
    }

    drush_log(dt("Successfully initialized a new dog instance in %worktree", array('%worktree' => $this->config['worktree']), 'success'));

    $old = substr(trim($this->gitPassthru('symbolic-ref HEAD', $this->config['worktree'])), 11);

    $this->gitPassthru('checkout -b ' . drush_escapeshellarg($this->config['localbranch']) . ' ' . drush_escapeshellarg($this->config['upstream_ref']), $this->config['worktree']);
    $this->gitPassthru('branch -D ' . $old, $this->config['worktree']);

    // Set up the collab remote, if the URI for it exists
    if (isset($this->config['git']['remote.collab.url'])) {

    }

    $collab = drush_get_option('collab', $this->config['git']['remote.collab.url']);
    $this->gitPassthru('remote add collab ' . drush_escapeshellarg($collab), $this->config['worktree']);

    // Init the .dog dir & sled
    mkdir($this->config['worktree'] . '/.dog');

    // Move the dog lib dir into place within the site instance
    $command = drush_get_command();
    $libpath = $command['path'] . '/lib/';
    drush_copy_dir($libpath, $this->config['worktree'] . '/.dog/lib');

    $this->gitPassthru('add -f -- .dog/lib', $this->config['worktree']);
    $this->gitPassthru('commit -m "Add dog class library." -o -- .dog/lib', $this->config['worktree']);

    // Init the files dir
    if (!file_exists($this->config['worktree'] . '/sites/default/files')) {
      mkdir($this->config['worktree'] . '/sites/default/files');
    }

    $gitignore = new \SplFileObject($this->config['worktree'] . '/sites/default/files/.gitignore', 'w');
    $gitignore->fwrite("*\n!.gitignore\n");

    // -o -- <filespec> doesn't work on untracked files; have to add it first
    $this->gitPassthru('add -f -- sites/default/files/.gitignore', $this->config['worktree']);
    $this->gitPassthru('commit -m "Add sites/default/files with a .gitignore file" -o -- sites/default/files/.gitignore', $this->config['worktree']);
  }
}

