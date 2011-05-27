<?php

class Dog_Sled {

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
