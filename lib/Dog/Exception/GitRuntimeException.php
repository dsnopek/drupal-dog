<?php

namespace Dog\Exception;

/**
 * Exception thrown when a runtime error occurs during the execution of a Git
 * command. Typically this means a non-0 exit status when 0 was expected.
 *
 */
class GitRuntimeException extends \RuntimeException implements BadDogInterface {}
