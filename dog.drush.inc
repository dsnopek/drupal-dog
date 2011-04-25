<?php

define('DRUSH_DOG_NO_GIT_BINARY', 151);

define('DRUSH_DOG_GIT_INVOCATION_ERROR', 152);

define('DRUSH_DOG_BAD_ENV_SETTINGS', 153);

define('DRUSH_DOG_INVALID_UPSTREAM', 154);

define('DRUSH_DOG_DIRTY_TREE', 155);

function dog_drush_help($section) {
  switch ($section) {
    case 'meta:dog:title':
      return dt('Drupal on Git (dog) commands');
    case 'meta:dog:summary':
      return dt('Build, develop, manage, and deploy an entire Drupal site using the dog methodology.');
  }
}

function dog_drush_command() {
  $items = array();

  $shared = array(
    'callback' => 'dog_command',
  );

  $items['dog-init'] = array(
    'description' => 'Set up a new dog-managed Drupal instance.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback arguments' => array('init'),
    'arguments' => array(
      'target-dir' => "The target directory where the instance should be initialized. Defaults to the upstream repository name if unspecified (same behavior as `git clone`).",
    ),
    'options' => array(
      'upstream' => 'The Drupal core repository to use as the upstream variant of Drupal. Accepts either a full URI or drush dl style input (e.g. "drupal-7.x"). If unspecified, defaults to vanilla core (git://git.drupal.org/project/drupal.git).',
      'upstream-ref' => 'If using a full git URI as the upstream, the branch or tag to start from.',
      'branch' => 'The name of the local branch to create for collaboration. Defaults to "master".',
      'collab' => 'The URI to use as the collab repository for this instance. If unspecified, the upstream URI is used.',
    ),
    'examples' => array(
      'drush dog-init --upstream=drupal-6.x --collab=git://github.com/me/my-project my_project' =>
        'Initialize a new Drupal 6.x project in a directory called "my_project" with the upstream from git.drupal.org and a collab repository on github.',
      'drush dog-init --upstream=git@gitorious.org:pressflow/6.git --upstream-ref=master --collab=file:///var/git/our_pressflow_project my_pressflow_project' =>
        'Initialize a new Pressflow project in "my_pressflow_project" from the master branch on a gitorious upstream, and a locally-hosted collab repository in /var/git.',
    ),
  ) + $shared;

  $items['dog-dl'] = array(
    'description' => 'Download contributed projects from Drupal.org via git cloning and register them into the dog system. This is essentially a wrapper on `drush dl` without the "--version-control*" and "--package-handler*" arguments, as those considerations are handled by dog.',
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'callback' => 'dog_command',
    'callback arguments' => array('normal'),
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
  ) + $shared;

  $items['dog-vet'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'description' => 'Inspect and report on the state of the current dog-managed Drupal instance, as it pertains to dog.',
  ) + $shared;

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
  ) + $shared;

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
  ) + $shared;

  $items['dog-upstream-update'] = array(
    'description' => 'Pull updates from upstream and commit them to the collab remote if one exists.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'arguments' => array(
      'target-dir' => 'Path to the module or theme directory, or drupal root where the collab should be added. Defaults to current working directory',
    ),
    'aliases' => array('dogup'),
  ) + $shared;

  $items['dog-remove'] = array(
    'description' => 'Remove a dog-managed extension from the site. This removes the attached git repository and removes all references to it from dog\'s metadata.',
    'arguments' => array(
      'target-dir' => 'Path to the module or theme directory, or drupal root where the collab should be added.',
    ),
    'aliases' => array('doggone'),
  ) + $shared;

  $items['dog-rollout'] = array(
    'description' => 'Roll out a new dog-managed Drupal instance either from an existing dog repository or a raw dog manifest.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'arguments' => array(
      'source' => 'A URI, resolving either to an already-built dog Git repository, or a raw dog manifest from which a new dog repository should be created.',
      'destination' => 'Path to the desired root of the instance. If unspecified, defaults to the `git-clone` behavior that inherits the local directory name from the remote URI; e.g., "git://...drupal.git" will be cloned into the relative path "drupal".',
    ),
    'aliases' => array('dogro'),
  ) + $shared;

  $items['dog-rollup'] = array(
    'description' => 'Update the dog manifests & metadata, or init them if they have not yet been created.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'options' => array(
      'no-commit' => 'Perform the roll up, but do not automatically commit the changes. By default, dog',
    ),
    'aliases' => array('dogru'),
  ) + $shared;

  $items['dog-revert'] = array(
    'description' => 'Roll back a dog resync operation.',
  ) + $shared;

  $items['dog-log'] = array(
    'description' => 'Wrapper on `git log` that helps filter log output to exclude upstream and irrelevant commit activity.',
  ) + $shared;

  return $items;
}

/**
 * Declare a custom package handler for dog.
 *
 * We need this for cleanliness; even if the git_drupalorg handler is improved,
 * we'll still probably want to keep this separate codepath.
 */
function dog_drush_engine_package_handler() {
  return array(
    'dog' => array(
      'package-handler=dog' => 'Dog\'s internal package handler. Used internally by Dog; will fail if attempts are made to use it directly.',
    ),
  );
}

/*
 * Dog utility functions
 */

/**
 * Determine whether the Git environment is adequate for dog's needs.
 *
 * We require a) an accessible, executable Git binary and b) that that binary
 * be at least version 1.7.
 *
 * @return bool
 *   Boolean indicating whether the Git environment is adequate.
 */
function _dog_verify_git() {
  try {
    $version = substr(trim(_dog_git_invoke('version')), 12);
  }
  catch (Exception $e) {
    $git_path = drush_get_option('git-path', 'git');
    if ($git_path === 'git') {
      drush_set_error(DRUSH_DOG_NO_GIT_BINARY, dt("No valid Git binary could be found on the PATH."), 'error');
    }
    else {
      drush_set_error(DRUSH_DOG_NO_GIT_BINARY, dt("A valid Git binary could not be located at '!path', as specified by your 'git-path' setting.", array('!path' => $git_path)), 'error');
    }
    return FALSE;
  }

  if (version_compare($version, '1.7', 'lt')) {
    drush_set_error('DRUSH_DOG_OLD_GIT_VERSION', dt('Your version of Git (!version) is too old; Dog requires version 1.7 or newer.', array('!version' => $version)));
    return FALSE;
  }

  return TRUE;
}

function _dog_git_invoke($command, $cwd = NULL, $repository_path = NULL, $fail_safe = FALSE, $env = NULL) {
  static $git_path, $env_source;

  // Inherit env using the sanest possible settings
  if (!isset($env_source)) {
    $vo = ini_get('variables_order');
    if (strpos($vo, 'E') !== FALSE) {
      $env_source =& $_ENV;
    }
    else if (strpos($vo, 'S') !== FALSE) {
      $env_source =& $_SERVER;
    }
    else {
      $env_source = FALSE;
    }
  }

  if ($env_source === FALSE) {
    return drush_set_error(DRUSH_DOG_BAD_ENV_SETTINGS, dt('Neither $_ENV nor $_SERVER are available to set up proper environment inheritance; ensure E and/or S is set in your php.ini\'s "variables_order" setting.'));
  }

  // Quick local static
  if (!isset($git_path)) {
    $git_path = drush_get_option('git-path', 'git');
  }

  if (!isset($env)) {
    $env = $env_source;
    if (isset($env['argv'])) {
      // At least clean out argv/argc from this proc if we must rely on $_SERVER
      unset($env['argv'], $env['argc']);
    }
  }
  if ($repository_path) {
    $env['GIT_DIR'] = $repository_path;
  }

  $descriptor_spec = array(
    1 => array('pipe', 'w'),
    2 => array('pipe', 'w'),
  );

  $process = proc_open("$git_path $command", $descriptor_spec, $pipes, $cwd, $env);
  if (is_resource($process)) {
    $stdout = stream_get_contents($pipes[1]);
    fclose($pipes[1]);
    $stderr = stream_get_contents($pipes[2]);
    fclose($pipes[2]);

    $return_code = proc_close($process);

    if ($return_code != 0 && !$fail_safe) {
      throw new Exception("Invocation of Git command '" . $command . "' failed with return code " . $return_code .": \n" . $stdout . $stderr);
    }

    return $stdout;
  }
}

function dog_command() {
  $args = func_get_args();
  $actual = array_shift($args); // First param is the actual command to run

  $command = drush_get_command();


  // No point in doing anything if a git binary can't be found
  if (_dog_verify_git()) {
    // Hacky way to indicate that we are operating in a proper dog environment.
    drush_set_context('DOG', TRUE);
    call_user_func_array('drush_invoke', array_merge(array($command['command-hook']), $args));
  }

  if (drush_get_error()) {
    drush_log(dt('Bad dog!'), 'notice');
  }
  else {
    drush_log(dt('Good dog!'), 'notice');
  }
}

/*
* Dog command callbacks
*/

function drush_dog_init($target = '') {
  $upstream = drush_get_option('upstream', 'drupal');

  if (strpos($upstream, 'drupal') === 0) {
    $version = pm_parse_project_version(array($upstream));
    $version = reset($version);
    $xml = _drush_pm_get_release_history_xml($version);
    $release = pm_parse_release($version, $xml);
    $upstream_ref = $release['tag'];
    $upstream = 'git://git.drupal.org/project/drupal.git';
  }
  else {
    $upstream_ref = drush_get_option('upstream-ref');
    if (!is_string($upstream_ref)) {
      return drush_set_error(DRUSH_DOG_INVALID_UPSTREAM, dt('An upstream ref must be specified as the starting point for the dog instance.'));
    }
  }

  $target = empty($target) ? preg_replace('/.*\/([\w]*)(\.git)?$/', '\1', $upstream) : $target;

  // Normalize target to absolute path
  $realtarget = $target[0] != '/' ? getcwd() . '/' . $target : $target;

  if (file_exists($realtarget)) {
    return drush_set_error(DRUSH_DOG_INVALID_UPSTREAM, dt("Target directory '$realtarget' already exists."));
  }

  try {
    $remoteinfo = _dog_git_invoke('ls-remote ' . drush_escapeshellarg($upstream) . ' 7.0');
  }
  catch (Exception $e) {
    return drush_set_error(DRUSH_DOG_INVALID_UPSTREAM, dt("Upstream URI '%upstream' is not a Git repository.", array('%upstream' => $upstream)));
  }

  // Check that the 7.0 tag is the SHA1 we know it should be.
  if (strpos($remoteinfo, '497914920385b7016ac9c9367e0198530787adf2') !== 0) {
    return drush_set_error(DRUSH_DOG_INVALID_UPSTREAM, dt("Upstream URI '%upstream' is not a valid Drupal core repository.", array('%upstream' => $upstream)));
  }

  // Build the base clone command.
  $cmd = 'clone -q -o upstream --no-hardlinks ';

  // Use a refcache if drush has one specified
  if ($refcache = drush_get_option('git-reference-cache')) {
    $cmd .= "--reference $refcache ";
  }

  // add the clone uri and target dir
  $cmd .= drush_escapeshellarg($upstream);

  // add the target output dir
  if (!empty($target)) {
    $cmd .= ' ' . drush_escapeshellarg($target);
  }

  drush_log(dt("Cloning '$upstream' into $realtarget."), 'info');

  try {
    // Do the clone
    _dog_git_invoke($cmd);
  }
  catch (Exception $e) {
    return drush_set_error(DRUSH_DOG_GIT_INVOCATION_ERROR, dt("Clone of upstream repository failed."));
  }

  drush_log(dt("Successfully initialized a new dog instance in $realtarget."), 'success');

  // Create the appropriate local branch based on the requested start point
  $localbranch = drush_get_option('branch', 'master');

  $old = substr(trim(_dog_git_invoke('symbolic-ref HEAD', $realtarget)), 11);

  _dog_git_invoke('checkout -b ' . drush_escapeshellarg($localbranch) . ' ' . drush_escapeshellarg($upstream_ref), $realtarget);
  _dog_git_invoke('branch -D ' . $old, $realtarget);

  // Set the collab uri
  $collab = drush_get_option('collab', $upstream);
  _dog_git_invoke('remote add collab ' . drush_escapeshellarg($collab), $realtarget);

  // Init the .dog dir & sled
  mkdir($realtarget . '/.dog');

  // For now at least, we just manually write a sledfile.
  $sledfile = new SplFileObject("$realtarget/.dog/sled", 'w+');

  $data = array(
    'mainRepo' => array(
      'upstream' => $upstream,
      'collab' => $collab,
    ),
  );

  $sledfile->fwrite(json_encode($data));
  _dog_git_invoke('add -f -- .dog/sled', $realtarget);
  _dog_git_invoke('commit -m "Add dog sled manifest file." -o -- .dog/sled', $realtarget);

  // Init the files dir
  if (!file_exists($realtarget . '/sites/default/files')) {
    mkdir($realtarget . '/sites/default/files');
  }

  $gitignore = new SplFileObject($realtarget . '/sites/default/files/.gitignore', 'w');
  $gitignore->fwrite("*\n!.gitignore\n");

  // -o -- <filespec> doesn't work on untracked files; have to add it first
  _dog_git_invoke('add -f -- sites/default/files/.gitignore', $realtarget);
  _dog_git_invoke('commit -m "Add sites/default/files with a .gitignore file" -o -- sites/default/files/.gitignore', $realtarget);
}

function drush_dog_dl() {
  // First ensure we have a clean working copy
  if (_dog_git_invoke('status -su no', DRUPAL_ROOT)) {
    // TODO perhaps we should allow this case, or at least provide better helptext
    return drush_set_error(DRUSH_DOG_DIRTY_TREE, dt('You have uncommitted changes in your working copy; dog-dl requires a clean working copy to run.'));
  }

  drush_set_option('package-handler', 'dog');
  if ($refcache = drush_get_option('git-reference-cache')) {
    drush_set_option('gitcloneparams', "--reference $refcache");
  }

  $args = func_get_args();

  if (empty($args)) {
    return drush_set_error(DRUSH_PM_REQUESTED_PROJECT_NOT_FOUND, dt('You must specify at least one project to download.'));
  }

  // TODO disallow core, drush, dog

  drush_invoke_args('pm-download', $args);
}
