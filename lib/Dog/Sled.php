<?php

namespace Dog;

use Dog\Repository\IRepository;
use Dog\Exception\ConcurrencyException;

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
   * An array of repository configuration objects, keyed by repository path.
   *
   * @var array
   */
  protected $repositoryConfigs = array();

  /**
   * The string name of the build target being used by the current dog instance.
   *
   * Until we actually do something with targets, this'll just fade into the
   * background; for now, it's here as a reminder that it's in the plan.
   *
   * @var string
   */
  protected $buildTarget;

  /**
   * Boolean to indicate whether the sled.xml file needs to be rewritten.
   *
   * This property is automatically set internally by some methods when they
   * detect a configuration change that must be written to disk.
   *
   * @var bool
   */
  protected $_needsWrite = FALSE;

  /**
   * Indicates whether the Sled needs to write itself to the on-disk sledfile.
   *
   * @return bool
   *   TRUE if a write is necesssary, FALSE if not.
   */
  public function needsWrite() {
    return $this->needsWrite;
  }

  public function __construct(Face $face) {
    $this->face = $face;
    $this->sled = new \SplFileObject($face->getBasePath() . "/.dog/sled.xml", 'c+');

    // git config files are *almost* standard ini file format, but the only time
    // a problem ought to show up is if/when an alias has unquoted disallowed
    // characters. Only aliases have any reason to have such values.
  }

  /**
   * Obtain a shared lock on the sledfile for the purposes of reading.
   */
  public function getReadLock() {
    // Acquire shared lock for reading. This lock is persisted throughout the
    // life time of this object.
    $this->sled->flock(LOCK_SH);
  }

  /**
   * Obtain an exclusive lock on the sledfile, for the purposes of writing.
   *
   * @param bool $exception
   * @throws Dog\Exception\ConcurrencyException
   */
  public function getWriteLock($exception = TRUE) {
    // Try for an exclusive lock, and cut everything short if we can't get one
    $this->sled->flock(LOCK_EX | LOCK_NB, $wouldblock); // Note - $wouldblock doesn't work on windoze
    if ($wouldblock && $exception) {
      throw new ConcurrencyException("Another process is already holds an exclusive lock (for writing) on the sledfile.", E_USER_ERROR, $previous);
    }
  }

  /**
   * Release all locks held on the sled file.
   */
  public function releaseLocks() {
    $this->sled->flock(LOCK_UN);
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
    $this->repositoryConfigs[$config['dog.repopath']] = $config;

    $this->_needsWrite = TRUE;
  }

  /**
   * Dump the contained configuration to the sled.xml file on disk.
   */
  public function dump() {
    $this->getWriteLock();

    $xmlstring = $this->generateSledXml();
    $this->sled->ftruncate(0);
    $this->sled->fwrite($xmlstring);

    $this->getReadLock();
  }

  /**
   * Transform all contained configuration into an XML string, ready to be
   * written to disk.
   *
   * @return string
   */
  protected function generateSledXml() {
    $xml = new \XMLWriter();
    $xml->openMemory();
    $xml->setIndent(TRUE);
    $xml->setIndentString('  ');
    $xml->startDocument('1.0', 'UTF-8');
    $xml->startElement('dogsled');

    foreach ($this->repositoryConfigs as $config) {
      $config->writeToXml($xml);
    }

    $xml->endElement();

    return $xml->outputMemory(TRUE);
  }

  public function __destruct() {
    if ($this->_needsWrite) {
      $this->dump();
    }
    $this->releaseLocks();
  }
}
