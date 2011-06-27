<?php

namespace Dog\Repository;

use Dog\Config\RepositoryConfig;

abstract class Base implements IRepository {

  /**
   * An instance of a repository configuration object. This object is used to
   *
   * @var Dog\Config\IRepository
   */
  protected $config;


  /**
   *
   * @var Dog\House\IHouse
   */
  protected $house;

  public function __construct(RepositoryConfig $config, IHouse $house) {
    $this->config = $config;
    $this->house = $house;
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

  public function gitPassthru($command, $exception = FALSE) {
    ;
  }
}

