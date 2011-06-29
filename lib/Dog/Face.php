<?php

namespace Dog;

use Dog\Repository\IRepository;
use Dog\Sled;
use Dog\Exception\BadDog;

/**
 * Dog's outermost interface. Attaching itself to the base of a Dog instance,
 * the DogFace acts as a gateway to all services and data in that instance.
 *
 * Acts as a broker, a factory, and a handful of other things, too.
 */
class Face {

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

  public function __construct($suggested_path = NULL, $init = FALSE) {
    if (TRUE === $init) {
      // We are init'ing something new. Go with provided path, or use cwd.
      $this->path = $suggested_path ?: getcwd();
      return;
    }

    if (NULL === $suggested_path) {
      // No path is provided. Try to suss our way out to the Dog root.
      if (defined('DRUPAL_ROOT')) {
        $suggested_path = DRUPAL_ROOT;
      }
      else {
        $msg = 'No DRUPAL_ROOT, no suggested path, and not setting up a new instance; cannot make a DogFace.';
        throw new BadDog($msg, E_RECOVERABLE_ERROR);
      }
    }

    $found = FALSE;
    $path = $suggested_path;
    while (FALSE === $found && '/' !== $path) {
      if (file_exists($path . '/sled')) {
        $found = TRUE;
        $this->path = $path;
      }
      $path = dirname($path);
    }

    if (FALSE === $found) {
      $msg = sprintf('No valid Dog instance could be found when climbing up towards root from %s.', $suggested_path);
      throw new BadDog($msg, E_RECOVERABLE_ERROR);
    }
  }

  public function getBasePath() {
    return $this->path;
  }

  public function getRepository($path) {
    return isset($this->repositories[$path]) ? $this->repositories[$path] : FALSE;
  }

  public function attachRepository(IRepository $repository) {

  }

  public function getSled() {
    if (isset($this->sled)) {
      $this->sled = new \Dog\Sled($this->path);
    }
    return $this->sled;
  }

  /**
   * Verify that this object represents a valid Dog instance.
   */
  public function verify() {
    return TRUE;
  }

  public static function gitExec() {
    
  }
}

