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

  /**
   * Convert the contents of this configuration object into an XML node,
   * typically for use in writing out the sled file.
   *
   * @param \XMLWriter $xml
   *  An already-in-progress XMLWriter, which expects
   */
  public function writeToXml(\XMLWriter $xml);

  /**
   * Populate this configuration object from a SimpleXMLElement.
   *
   * @param \SimpleXMLElement $xml
   */
  public function buildFromXml(\SimpleXMLIterator $xml);
}
