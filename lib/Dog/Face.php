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
   * Boolean flag indicating whether this DogFace is acting in initialization
   * capacity.
   *
   * Initialization encompasses two commands: dog-init and dog-rollout. In these
   * cases, the DogFace object needs to be instantiated before any repositories
   * exist on disk, and before a sled exists (for init) or is accessible (for
   * rollout).
   *
   * Some DogFace actions are only available when in an initialization state,
   * while some others are only available when in a non-initialization state.
   *
   * Defaults to FALSE.
   *
   * @var bool
   */
  protected $init = FALSE;

  /**
   *
   * @var DogSled
   */
  protected $sled;

  protected $path;

  /**
   * A path from which the DogFace should start its search (by climbing upwards
   * towards root) for a valid Dog instance, as indicated by the presence of a
   * sled.
   *
   * @var string
   */
  protected $suggestedPath;

  /**
   * An associative array of objects implementing DogRepositoryInterface.
   * This set of objects represents all repositories known to this Dog instance.
   *
   * @var type
   */
  protected $repositories = array();

  /**
   * Construct a new Dog\Face object.
   *
   * FIXME the flow here is awkward wrt handling the init case.
   *
   * @param string $suggested_path
   * @param bool $init
   * @return \Dog\Face
   */
  public function __construct($suggested_path = NULL, $init = FALSE) {
    $this->init = $init;
    if (TRUE === $init) {
      // We are init'ing something new. Go with provided path, or use cwd, and
      // skip all other initialization logic.
      return $this->path = $suggested_path ?: getcwd();
    }

    if (NULL === $suggested_path) {
      // No path is provided. Try to suss our way out to the Dog root.
      if (defined('DRUPAL_ROOT')) {
        $this->suggestedPath = DRUPAL_ROOT;
      }
      else {
        $this->suggestedPath = getcwd();
//        $msg = 'No DRUPAL_ROOT, no suggested path, and not setting up a new instance; cannot make a DogFace.';
//        throw new BadDog($msg, E_RECOVERABLE_ERROR);
      }
    }
  }

  /**
   * Return the root path of the current Dog instance.
   *
   * @return string
   */
  public function getBasePath() {
    if (NULL === $this->path) {
      $found = FALSE;
      $path = $this->suggestedPath;
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

