<?php

namespace IPP\Student;

class MyTrue extends MyBool
{
    private static ?MyTrue $instance = null;

    private function __construct() {}

    public static function getInstance(): MyTrue
    {
        if (self::$instance === null) {
            self::$instance = new MyTrue();
        }
        return self::$instance;
    }

    public function not(): MyBool
    {
        return MyFalse::getInstance();
    }

    public function and(callable $block): MyBool
    {
        return $block();
    }

    public function or(callable $block): MyBool
    {
        return $block();
    }

    public function ifTrueIfFalse(callable $ifTrue, callable $ifFalse) : MyBool
    {
        return $ifTrue();
    }

    public function __toString(): string
    {
        return 'true';
    }
}