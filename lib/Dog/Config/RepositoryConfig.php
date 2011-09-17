<?php

namespace Dog\Config;

/**
 * A configuration object that is used by Dog\Repository\RepositoryInterface objects.
 *
 */
use Dog\Exception\MissingConfigurationException;

class RepositoryConfig implements ConfigInterface {
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
    foreach (array('worktree', 'repopath', 'upstream_ref') as $item) {
      if (!isset($this->conf[$item])) {
        throw new MissingConfigurationException('Missing a required Dog-specific configuration key for using a repository.', E_RECOVERABLE_ERROR, NULL, $item);
      }
    }
    foreach (array('remote.upstream.url') as $item) {
      if (!isset($this->conf['git'][$item])) {
        throw new MissingConfigurationException('Missing a required git-internal configuration key for using a repository.', E_RECOVERABLE_ERROR, NULL, $item);
      }
    }
    return TRUE;
  }

  public function getConf() {
    return $this->conf;
  }

  public function writeToXml(\XMLWriter $xml) {
    $writer = function(&$xml, $conf) use (&$writer) {
      foreach ($conf as $key => $value) {
        if (!is_array($value)) {
          $xml->writeElement($key, $value);
        }
        else {
          $xml->startElement($key);
          $writer($xml, $value);
          $xml->endElement();
        }
      }
    };

    $xml->startElement('repository');
    $writer($xml, $this->conf);
    $xml->endElement();
  }

  public function buildFromXml(\SimpleXMLIterator $xml) {
    $build = function(\SimpleXMLIterator &$xml) use (&$build) {
      $conf = array();
      foreach ($xml as $key => $value) {
        if ($value->hasChildren()) {
          $conf[$key] = $build($xml);
        }
        else {
          $conf[$key] = strval($value);
        }
      }
      return $conf;
    };

    $this->conf = $build($xml);
  }

  // Implementation of ArrayAccess methods

  public function offsetExists($offset) {
    return array_key_exists($offset, $this->conf);
  }

  public function &offsetGet($offset) {
    return $this->conf[$offset];
  }

  public function offsetSet($offset, $value) {
    $this->conf[$offset] = $value;
  }

  public function offsetUnset($offset) {
    unset($this->conf[$offset]);
  }
}
