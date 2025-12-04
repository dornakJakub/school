<?php

namespace IPP\Student;

class MyNil extends MyObjectInstance
{
    private static ?MyNil $instance = null;

    private function __construct() {}

    public static function getInstance(): MyNil
    {
        if (self::$instance === null) {
            self::$instance = new MyNil();
        }
        return self::$instance;
    }

    public function isNil()
    {
        return MyTrue::getInstance();
    }

    public function asString()
    {
        return new MyString('nil');
    }
}