<?php

namespace Dog\Repository;

/**
 * Interface defining general behaviors for all Dog repositories, regardless of
 * type.
 */
interface IRepository {

  /**
   *
   * @var mixed
   */
  public function getlastStderr();

  public function getlastStdout();

  public function getlastExit();

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
   */
  public function gitPassthru($command, $exception = FALSE);

  public function getHookSet();

  public function setHookSet($name);

  public function getRemoteInfo($name);

  public function setRemoteInfo($name, $info);

  /**
   * Represent this repository object as a string containing the absolute path
   * to the repository on disk.
   */
  public function __toString();
}
