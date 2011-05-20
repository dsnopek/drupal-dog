<?php

interface DogHouseInterface {
  public function getRepository($path);

  public function attachRepository(DogRepositoryInterface $repository);

  public function verify();
}

class DogHouse implements DogHouseInterface {

  /**
   *
   * @var DogSled
   */
  protected $sled;

  protected $path;

  public function __construct($path) {
    $this->path = $path;
  }

  public function getRepository($path) {

  }

  public function attachRepository(DogRepositoryInterface $repository) {
    
  }
}

class DogSled {

  /**
   *
   * @var SplFileObject
   */
  protected $sled;

  protected $mainRepoConfig;

  protected $attachedRepoConfigs = array();

  /**
   * The string name of the build target being used by the current dog instance.
   *
   * Until we actually do something with targets, this'll just fade into the
   * background; for now, it's here as a reminder that it's in the plan.
   *
   * @var string
   */
  protected $buildTarget;

  public function __construct($base_path) {
    $this->sled = new SplFileObject("$base_path/.dog/sled", 'a+');

    // git config files are *almost* standard ini file format, but the only time
    // a problem ought to show up is if/when an alias has unquoted disallowed
    // characters. Only aliases have any reason to have such values.
    $this->mainRepoConfig = parse_ini_file("$base_path/.git/config", TRUE);
    $this->attachedRepoConfigs = array();
  }

  public function getBuildTarget() {
    if (is_null($this->buildTarget)) {
      $this->buildTarget = isset($this->mainRepoConfig['dog']['buildTarget']) ? $this->mainRepoConfig['dog']['buildTarget'] : 'default';
    }

    return $this->buildTarget;
  }

  public function dump() {
    $this->sled->flock(LOCK_EX, TRUE);
    $this->sled->ftruncate(0);
    $this->sled->fwrite($this);
    $this->sled->flock(LOCK_UN, TRUE);
  }

  public function __toString() {
    $obj = new stdClass();
    $obj->mainRepoConfig = $this->mainRepoConfig;
    $obj->attachedRepoConfigs = $this->attachedRepoConfigs;
    return json_encode($obj);
  }
}


/**
 * Interface defining general behaviors for all Dog repositories, regardless of
 * type.
 */
interface DogRepositoryInterface {

  /**
   *
   * @var mixed
   */
  public $lastStderr;

  public $lastStdout;

  public $lastExit;

  /**
   * Return the branch currently checked out in this repository.
   * 
   * @param bool $name_only
   *   Whether or not the leading 'refs/heads/' should be pruned from the
   *   return value.
   * @return mixed
   *   Returns a string containing the name of the current branch, or FALSE if
   *   the repository is currently in a detached HEAD state.
   */
  public function getCurrentBranch($name_only = TRUE);

  /**
   * Execute a Git command against this repository.
   */
  public function gitPassthru($command, $exception = FALSE);

  public function getHookSet();

  public function setHookSet($name);

  public function getRemoteInfo($name);

  public function setRemoteInfo($name, $info);

  /**
   * Represent this repository object as a string containing the absolute path
   * to the repository on disk.
   */
  public function __toString();
}

abstract class DogRepositoryBase implements DogRepositoryInterface {

  public function getCurrentBranch($name_only = TRUE) {
    try {
      $current = trim($this->gitPassthru('symbolic-ref -q HEAD', TRUE));
      return $name_only ? substr($current, 12): $current;
    }
    catch (Exception $e) {
      return FALSE;
    }
  }

  public function gitPassthru($command, $exception = FALSE) {
    ;
  }
}

class DogMainRepository extends DogRepositoryBase {

}


abstract class DogRepo implements DogRepoInterface {
  public function __construct($repo_path) {

  }
}
