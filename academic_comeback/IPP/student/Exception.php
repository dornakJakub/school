<?php

namespace IPP\Student;

use IPP\Core\ReturnCode;
use IPP\Core\Exception\IPPException;
use Throwable;

class Exception extends IPPException
{
    public function __construct(string $message = "Internal error", int $returnCode = ReturnCode::INTERNAL_ERROR, ?Throwable $previous = null)
    {
        parent::__construct($message, $returnCode, $previous, false);
    }
}