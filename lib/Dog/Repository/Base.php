<?php

namespace Dog\Repository;

use Dog\Config\RepositoryConfig;
use Dog\Face;

abstract class Base implements IRepository {

  /**
   * An instance of a repository configuration object. This object is used to
   *
   * @var Dog\Config\IRepository
   */
  protected $config;


  /**
   *
   * @var Dog\Face
   */
  protected $face;

  public function __construct(RepositoryConfig $config, Face $face) {
    $this->config = $config;
    $this->face = $face;
  }

  public function create() {
    $path = $this->config->getPath;
  }

  public function getCurrentBranch($name_only = TRUE) {
    try {
      $current = trim($this->gitPassthru('symbolic-ref -q HEAD', TRUE));
      return $name_only ? substr($current, 12): $current;
    }
    catch (Exception $e) {
      return FALSE;
    }
  }

  public function gitPassthru($command, $cwd = NULL, $fail_safe = FALSE, $env = NULL) {
    static $env_source;

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

    if (NULL === $cwd) {
      // Set the cwd to the working copy path, but only if it exists. This
      // should cover us for the core.worktree AND newly cloning case.
      $cwd = file_exists($this->config['dog.worktree']) ? $this->config['dog.worktree'] : $this->face->getBasePath();
    }

    if ($env_source === FALSE) {
      return drush_set_error('DRUSH_DOG_BAD_ENV_SETTINGS', dt('Neither $_ENV nor $_SERVER are available to set up proper environment inheritance; ensure E and/or S is set in your php.ini\'s "variables_order" setting.'));
    }

    if (!isset($env)) {
      $env = $env_source;
      if (isset($env['argv'])) {
        // If we must rely on $_SERVER, at least clean out argv/argc
        unset($env['argv'], $env['argc']);
      }
    }

    if ($this->config['dog.repopath']) {
      $env['GIT_DIR'] = $this->config['dog.repopath'];
    }

    $descriptor_spec = array(
      1 => array('pipe', 'w'),
      2 => array('pipe', 'w'),
    );

    // proc_open() and $pipes var preclude using drush_op() so we simulate it.
    if (drush_get_context('DRUSH_VERBOSE') || drush_get_context('DRUSH_SIMULATE')) {
       drush_print("Calling proc_open(git $command)");
    }

    if (!drush_get_context('DRUSH_SIMULATE')) {
      $process = proc_open("git $command", $descriptor_spec, $pipes, $cwd, $env);
      if (is_resource($process)) {
        $stdout = stream_get_contents($pipes[1]);
        fclose($pipes[1]);
        $stderr = stream_get_contents($pipes[2]);
        fclose($pipes[2]);
      }

      $return_code = proc_close($process);

      if ($return_code != 0 && !$fail_safe) {
        throw new \Exception(sprintf("Invocation of Git command '%s' failed with return code %d:\n%s\n\n%s\n\n", $command, $return_code, $stdout, $stderr), E_RECOVERABLE_ERROR);
      }

      return $stdout;
    }
  }

  public function __toString() {
    return $this->face->getBasePath() . $this->config['dog.repopath'];
  }

  public function getConfig() {
    return $this->config;
  }
}
