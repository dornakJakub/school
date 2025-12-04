<?php

namespace IPP\Student;

abstract class MyBool extends MyObjectInstance
{
    abstract public function not(): MyBool;

    abstract public function and(callable $block): MyBool;

    abstract public function or(callable $block): MyBool;

    /**
     * @return MyBool
     */
    abstract public function ifTrueIfFalse(callable $ifTrue, callable $ifFalse);

    abstract public function __toString();
}