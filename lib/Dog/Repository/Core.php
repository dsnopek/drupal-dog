<?php

namespace Dog\Repository;

/**
 * The repository representing the core clone that sits at the heart of every
 * Dog instance.
 * 
 */
class Core extends Base {
  public function create() {
    if (!$this->config->ensure()) {
      drush_set_error('DRUSH_DOG_MISSING_DATA', dt("Repository configuration object is missing some vital data."));
      return FALSE;
    }

    try {
      $remoteinfo = _dog_git_invoke('ls-remote ' . drush_escapeshellarg($this->config['remote.upstream.url']) . ' 7.0');
    }
    catch (Exception $e) {
      return drush_set_error('DRUSH_DOG_INVALID_UPSTREAM', dt("Upstream URI '%upstream' is not a Git repository.", array('%upstream' => $this->config['remote.upstream.url'])));
    }

    // Check that the 7.0 tag is the SHA1 we know it should be.
    if (strpos($remoteinfo, '497914920385b7016ac9c9367e0198530787adf2') !== 0) {
      return drush_set_error('DRUSH_DOG_INVALID_UPSTREAM', dt("Upstream URI '%upstream' is not a valid Drupal core repository.", array('%upstream' => $this->config['remote.upstream.url'])));
    }

    // Build the base clone command.
    $cmd = 'clone -q -o upstream --no-hardlinks ';

    // Use a refcache if drush has one specified
    if ($refcache = drush_get_option('git-reference-cache')) {
      $cmd .= "--reference $refcache ";
    }

    // add the clone uri and target dir
    $cmd .= drush_escapeshellarg($this->config['remote.upstream.url']);

    // add the target output dir
    $cmd .= ' ' . drush_escapeshellarg($this->config['dog.worktree']);

    drush_log(dt("Cloning '%upstream' into %realtarget.", array('%upstream' => $this->config['remote.upstream.url'], '%realtarget' => $this->config['dog.worktree'])), 'info');

    try {
      // Do the clone
      _dog_git_invoke($cmd);
    }
    catch (Exception $e) {
      return drush_set_error('DRUSH_DOG_GIT_INVOCATION_ERROR', dt("Clone of upstream repository failed."));
    }

    drush_log(dt("Successfully initialized a new dog instance in %worktree", array('%worktree' => $this->config['dog.worktree']), 'success'));

    // Create the appropriate local branch based on the requested start point
    $localbranch = drush_get_option('branch', 'master');

    $old = substr(trim(_dog_git_invoke('symbolic-ref HEAD', $this->config['dog.worktree'])), 11);

    _dog_git_invoke('checkout -b ' . drush_escapeshellarg($localbranch) . ' ' . drush_escapeshellarg($this->config['tmp']['upstream_ref']), $this->config['dog.worktree']);
    _dog_git_invoke('branch -D ' . $old, $this->config['dog.worktree']);

    // Set the collab uri
    $collab = drush_get_option('collab', $this->config['remote.upstream.url']);
    _dog_git_invoke('remote add collab ' . drush_escapeshellarg($collab), $this->config['dog.worktree']);

    // Init the .dog dir & sled
    mkdir($this->config['dog.worktree'] . '/.dog');

    // Move the dog lib dir into place within the site instance
    $command = drush_get_command();
    $libpath = $command['path'] . '/lib/';
    drush_copy_dir($libpath, $this->config['dog.worktree'] . '/.dog/lib');

    _dog_git_invoke('add -f -- .dog/lib', $this->config['dog.worktree']);
    _dog_git_invoke('commit -m "Add dog class library." -o -- .dog/lib', $this->config['dog.worktree']);

    // For now at least, we just manually write a sledfile.
    $sledfile = new \SplFileObject($this->config['dog.worktree'] . '/.dog/sled', 'w+');

    $data = array(
      'mainRepo' => array(
        'upstream' => $this->config['remote.upstream.url'],
        'collab' => $collab,
      ),
    );

    $sledfile->fwrite(json_encode($data));
    _dog_git_invoke('add -f -- .dog/sled', $this->config['dog.worktree']);
    _dog_git_invoke('commit -m "Add dog sled manifest file." -o -- .dog/sled', $this->config['dog.worktree']);

    // Init the files dir
    if (!file_exists($this->config['dog.worktree'] . '/sites/default/files')) {
      mkdir($this->config['dog.worktree'] . '/sites/default/files');
    }

    $gitignore = new \SplFileObject($this->config['dog.worktree'] . '/sites/default/files/.gitignore', 'w');
    $gitignore->fwrite("*\n!.gitignore\n");

    // -o -- <filespec> doesn't work on untracked files; have to add it first
    _dog_git_invoke('add -f -- sites/default/files/.gitignore', $this->config['dog.worktree']);
    _dog_git_invoke('commit -m "Add sites/default/files with a .gitignore file" -o -- sites/default/files/.gitignore', $this->config['dog.worktree']);
  }
}

