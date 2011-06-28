<?php

namespace Dog\House;

use Dog\Repository;

interface IHouse {
  public function getRepository($path);

  public function getBasePath();

  public function attachRepository(IRepository $repository);

  public function verify();
}

