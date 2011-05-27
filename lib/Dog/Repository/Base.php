<?php

abstract class Dog_Repository_Base implements Dog_Repository_Interface {

  public function getCurrentBranch($name_only = TRUE) {
    try {
      $current = trim($this->gitPassthru('symbolic-ref -q HEAD', TRUE));
      return $name_only ? substr($current, 12): $current;
    }
    catch (Exception $e) {
      return FALSE;
    }
  }

  public function gitPassthru($command, $exception = FALSE) {
    ;
  }
}

