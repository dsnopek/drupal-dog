<?php

/**
 * Dog's outermost interface. Attaching itself to the base of a Dog instance,
 * the DogHouse acts as a gateway to all services and data in that instance.
 *
 * Acts as a broker, a factory, and a handful of other things, too.
 */
class Dog_House_Default implements Dog_House_Interface {

  /**
   *
   * @var DogSled
   */
  protected $sled;

  protected $path;

  /**
   * An associative array of objects implementing DogRepositoryInterface.
   * This set of objects represents all repositories known to this Dog instance.
   *
   * @var type
   */
  protected $repositories = array();

  public function __construct($path) {
    $this->path = $path;
    $this->sled = new Dog_Sled($path);
  }

  public function getRepository($path) {
    return isset($this->repositories[$path]) ? $this->repositories[$path] : FALSE;
  }

  public function attachRepository(Dog_Repository_Interface $repository) {

  }

  /**
   * Verify that this object represents a valid Dog instance.
   */
  public function verify() {
    return TRUE;
  }
}

