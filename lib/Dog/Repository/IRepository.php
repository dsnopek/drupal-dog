<?php

namespace Dog\Repository;

use Dog\Config\RepositoryConfig;
use Dog\Face;

/**
 * Interface defining general behaviors for all Dog repositories, regardless of
 * type.
 */
interface IRepository {

  /**
   * Create a new Git repository-handling object.
   *
   * We use constructor dependency injection to provide config information about
   * the repository using a Dog\Config\Repository object. This makes it easy to
   * combine the logic for new and existing repositories into a single class.
   *
   * Note that the information embedded within the config object
   * should be expected/desired values, which may not be the same as the on-disk
   * values at the time of object instantiation. That is, in the case where the
   * repository already exists, the config object should contain the values
   * recorded in the Sled, which may differ from the real values for the
   * repository on disk. In the case where a new repository is to be created,
   * the config object should contain the desired values to be used in creating
   * and any subsequent setup of the repository.
   *
   * This usage pattern, where Dog\Config\IConfig objects are populated from
   * the sled in the event of an existing resource, or with desired values in
   * the event of a new resource, is the expected pattern throughout dog.
   *
   * @param Dog\Config\RepositoryConfig $config
   *   A Dog\Config\Repository object, which implements Dog\Config\IConfig, and
   *   contains all vital configuration information for this repository.
   *
   * @param Dog\Face $face
   *   The object implementing Dog\Face that is being used to manage
   *   this Dog instance.
   */
  public function __construct(RepositoryConfig $config, Face $face);

  /**
   * Create a new repository using configuration information already passed in
   * via the constructor.
   *
   * @return bool
   *   TRUE on success, FALSE on failure.
   */
  public function create();

  /**
   *
   * @var mixed
   */

  /**
   * Return the branch currently checked out in this repository.
   *
   * @param bool $name_only
   *   Whether or not the leading 'refs/heads/' should be pruned from the
   *   return value.
   * @return mixed
   *   Returns a string containing the name of the current branch, or FALSE if
   *   the repository is currently in a detached HEAD state.
   */
  public function getCurrentBranch($name_only = TRUE);

  /**
   * Execute a Git command against this repository.
   *
   * This command respects drush's various output & execution control options.
   * Or at least it should - if it doesn't, file a bug :)
   */
  public function gitPassthru($command, $cwd = NULL, $fail_safe = FALSE, $env = NULL);

//  public function getHookSet();
//
//  public function setHookSet($name);

//  public function getRemoteInfo($name);
//
//  public function setRemoteInfo($name, $info);

  /**
   * Represent this repository object as a string containing the absolute path
   * to the repository on disk.
   */
  public function __toString();
}
