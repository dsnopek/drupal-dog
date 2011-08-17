<?php

namespace Dog\Config;

/**
 * Interface governing all sleddable configuration objects.
 */
interface IConfig extends \ArrayAccess {

  /**
   * Ensure that at least the basic values required for this configuration
   * object to be useful are present.
   *
   * @return bool
   */
  public function ensure();

  /**
   * Return the entire configuration array.
   *
   * Typically used when dumping out an array to the sled.
   *
   * @return array
   */
  public function getConf();
}


