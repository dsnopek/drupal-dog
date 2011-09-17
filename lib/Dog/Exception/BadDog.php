<?php

namespace Dog\Exception;

/**
 * Generic Dog exception, used when no other Exception fits the bill (as an
 * alternative to the generic Exception class).
 *
 */
class BadDog extends \Exception implements BadDogInterface {}

