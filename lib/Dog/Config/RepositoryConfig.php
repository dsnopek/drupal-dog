<?php

namespace Dog\Config;

/**
 * A configuration object that is used by Dog\Repository\IRepository objects.
 *
 */
class RepositoryConfig implements IConfig {
  protected $conf = array();

//  /**
//   * Return the path, relative to the Dog root, where this repository lives.
//   */
//  public function getRepositoryPath();
//
//  public function setRepositoryPath();
//
//  /**
//   * Return the path, relative to the Dog root, where the worktree of this
//   * repository lives.
//   *
//   */
//  public function getWorkTreePath();
//
//  public function setWorkTreePath();

  /**
   * Ensure that we have at least the basic values required for making a
   * repository operate.
   */
  public function ensure() {
    foreach (array('dog.worktree', 'dog.repopath', 'dog.upstream_ref', 'remote.upstream.url') as $item) {
      if (!isset($this->conf[$item])) {
        // TODO replace with exceptions
        return FALSE;
      }
    }
    return TRUE;
  }

  public function getConf() {
    return $this->conf;
  }

  // Implementation of ArrayAccess methods

  public function offsetExists($offset) {
    return array_key_exists($offset, $this->conf);
  }

  public function offsetGet($offset) {
    return array_key_exists($offset, $this->conf) ? $this->conf[$offset] : FALSE;
  }

  public function offsetSet($offset, $value) {
    $this->conf[$offset] = $value;
  }

  public function offsetUnset($offset) {
    unset($this->conf[$offset]);
  }
}
