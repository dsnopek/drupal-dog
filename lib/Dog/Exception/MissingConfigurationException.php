<?php

namespace Dog\Exception;

/**
 * Exception indicating an expected configuration value was not present.
 *
 */
class MissingConfigurationException extends \RuntimeException implements BadDogInterface {
  /**
   * The expected-but-absent configuration key.
   *
   * @var string
   */
  protected $missingKey;

  public function __construct($message = '', $code = 0, Exception $previous = NULL, $missingkey) {
    $this->missingKey = $missingkey;
    parent::__construct($message, $code, $previous);
  }

  public function getMissingKey() {
    return $this->missingKey;
  }
}