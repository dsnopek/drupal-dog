<?php

namespace Dog;

class Sled {

  /**
   *
   * @var SplFileObject
   */
  protected $sled;

  /**
   * The Dog\Face representing the dog run by this sled.
   *
   * @var Dog\Face
   */
  protected $face;

  /**
   * The full config tree, as contained in the sledfile.
   * 
   * @var type
   */
  protected $config;

  /**
   * The string name of the build target being used by the current dog instance.
   *
   * Until we actually do something with targets, this'll just fade into the
   * background; for now, it's here as a reminder that it's in the plan.
   *
   * @var string
   */
  protected $buildTarget;

  public function __construct(Face $face) {
    $this->face = $face;
    $this->sled = new \SplFileObject($face->getBasePath() . "/.dog/sled", 'a+');

    $this->config = json_decode($this->sled, TRUE);

    // git config files are *almost* standard ini file format, but the only time
    // a problem ought to show up is if/when an alias has unquoted disallowed
    // characters. Only aliases have any reason to have such values.
  }

  /**
   * Retrieves the build target this dog instance is operating against.
   *
   * TODO This is kinda more of a stub method, really, as build targeting hasn't
   * been implemented yet. At all.
   *
   * @return string
   */
  public function getBuildTarget() {
    if (NULL === $this->buildTarget) {
      $this->buildTarget = isset($this->mainRepoConfig['dog']['buildTarget']) ? $this->mainRepoConfig['dog']['buildTarget'] : 'default';
    }

    return $this->buildTarget;
  }

  public function attachNewRepository(IRepository $repository) {
    $config = $repository->getConfig();
    $type = get_class($repository);
    $config['dog.repoClass'] = $type;
    $this->config['repositories'][$config['dog.repopath']] = $config->getConf();
  }

  public function dump() {
    $this->sled->flock(LOCK_EX, TRUE);
    $this->sled->ftruncate(0);
    $this->sled->fwrite($this);
    $this->sled->flock(LOCK_UN, TRUE);
  }

  public function __toString() {
    return json_encode($this->config);
  }
}
