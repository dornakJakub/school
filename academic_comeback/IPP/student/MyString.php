<?php

namespace IPP\Student;

use IPP\Student\IOManager;

class MyString extends MyObjectInstance
{
    /**
     * @var string $value
     */
    private $value;

    /**
     * @param string $value
     */
    public function __construct($value = '')
    {
        $this->value = $value;
    }

    /**
     * @param string $value
     */
    public function setValue($value): void
    {
        $this->value = $value;
    }

    public function getValue(): string
    {
        return $this->value;
    }

    public static function read(): MyString
    {
        return new MyString(IOManager::getStdin()->readString());
    }

    public function isString(): MyBool
    {
        return MyTrue::getInstance();
    }

    public function print(): MyString
    {
        IOManager::getStdout()->writeString($this->value);

        return $this;
    }

    /**
     * @param MyString $string
     * @return MyBool
     */
    public function equalTo($string): MyBool
    {
        if ($this->value === $string[0]->getValue()) {
            return MyTrue::getInstance();
        }

        return MyFalse::getInstance();
    }

    public function asString(): MyString
    {
        return $this;
    }

    /**
    * @return MyInt|MyNil
    */
    public function asInteger()
    {
        if (filter_var($this->value, FILTER_VALIDATE_INT) !== false)
            return new MyInt((int)$this->value);
        
        return MyNil::getInstance();
    }

    /**
     * @param MyString $string
     * @return MyString|MyNil
     */
    public function concatenateWith($string)
    {
        if ($string[0] instanceof MyString) {
            return new MyString($this->value . $string[0]->getValue());
        }

        return MyNil::getInstance();
    }

    /**
     * @param array<MyInt, MyInt> $args
     * @return MyString|MyNil
     */
    public function startsWithEndsBefore($args)
    {
        $starts = $args[0]->getValue() - 1;
        $ends = $args[1]->getValue() - 1;
        if ($starts < 0 || $ends < 0) {
            return MyNil::getInstance();
        } else if ($ends - $starts <= 0) {
            return new MyString('');
        } else {
            $substring = substr($this->value, $starts, $ends - $starts);
            return new MyString($substring);
        }
    }
}   