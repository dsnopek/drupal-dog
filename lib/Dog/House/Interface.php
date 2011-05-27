<?php

interface Dog_House_Interface {
  public function getRepository($path);

  public function attachRepository(Dog_Repository_Interface $repository);

  public function verify();
}

