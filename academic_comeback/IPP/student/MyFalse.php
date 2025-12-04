<?php

namespace IPP\Student;

class MyFalse extends MyBool
{
    private static ?MyFalse $instance = null;

    private function __construct() {}

    public static function getInstance(): MyFalse
    {
        if (self::$instance === null) {
            self::$instance = new MyFalse();
        }
        return self::$instance;
    }

    public function not(): MyBool
    {
        return MyTrue::getInstance();
    }

    public function and(callable $block): MyBool
    {
        return $block();
    }

    public function or(callable $block): MyBool
    {
        return $block();
    }

    /**
     * @return MyBool
     */
    public function ifTrueIfFalse(callable $ifTrue, callable $ifFalse)
    {
        return $ifFalse();
    }

    public function __toString(): string
    {
        return 'false';
    }
}