<?php

namespace IPP\Student;

use IPP\Student\Exception;
use IPP\Core\ReturnCode;
use IPP\Student\IOManager;

class MyInt extends MyObjectInstance
{
    /**
     * @var int $value
     */
    private $value;

    public function __construct($value = 0)
    {
        $this->value = $value;
        // echo "Creating MyInt with value:" .  json_encode($value) . "\n";
    }

    public function setValue($value)
    {
        $this->value = $value;
    }

    public function getValue()
    {
        return $this->value;
    }

    public function isNumber()
    {
        return MyTrue::getInstance();
    }

    public function equalTo($number)
    {
        if (filter_var($number[0]->getValue(), FILTER_VALIDATE_INT) === false)
            throw new Exception("Invalid operand type", ReturnCode::INTERPRET_VALUE_ERROR);
        if ($this->value === $number[0]->getValue()) {
            return MyTrue::getInstance();
        }

        return MyFalse::getInstance();
    }

    /**
     * @param MyInt $number
     * @return MyBool
     */
    public function greaterThan($number)
    {
        if (filter_var($number[0]->getValue(), FILTER_VALIDATE_INT) === false)
            throw new Exception("Invalid operand type", ReturnCode::INTERPRET_VALUE_ERROR);
        if ($this->value > $number[0]->getValue()) {
            return MyTrue::getInstance();
        }

        return MyFalse::getInstance();
    }

    /**
     * @param MyInt $number
     * @return MyInt
     */
    public function plus($number)
    {
        if (filter_var($number[0]->getValue(), FILTER_VALIDATE_INT) === false)
            throw new Exception("Invalid operand type", ReturnCode::INTERPRET_VALUE_ERROR);
        return new MyInt($this->value + $number[0]->getValue());
    }

    /**
     * @param MyInt $number
     * @return MyInt
     */
    public function minus($number)
    {
        if (filter_var($number[0]->getValue(), FILTER_VALIDATE_INT) === false)
            throw new Exception("Invalid operand type", ReturnCode::INTERPRET_VALUE_ERROR);
        return new MyInt($this->value - $number[0]->getValue());
    }

    /**
     * @param MyInt $number
     * @return MyInt
     */
    public function multiplyBy($number)
    {
        if (filter_var($number[0]->getValue(), FILTER_VALIDATE_INT) === false)
            throw new Exception("Invalid operand type", ReturnCode::INTERPRET_VALUE_ERROR);
        return new MyInt($this->value * $number[0]->getValue());
    }

    /**
     * @param MyInt $number
     * @return MyInt
     */
    public function divBy($number)
    {
        if (filter_var($number[0]->getValue(), FILTER_VALIDATE_INT) === false)
            throw new Exception("Invalid operand type", ReturnCode::INTERPRET_VALUE_ERROR);
        if ($number[0]->getValue() == 0) {
            throw new Exception("Division by zero", ReturnCode::INTERPRET_VALUE_ERROR);
        }

        return new MyInt(intdiv($this->value, $number[0]->getValue()));
    }

    public function asString() {
        return new MyString((string)$this->value);
    }

    /**
     * @return MyInt
     */
    public function asInteger() {
        return $this;
    }

    /**
     * @return MyInt
     * @param callable $block
     */
    public function timesRepeat($block) {
        if ($this->value < 0) {
            throw new Exception("Negative repeat count", ReturnCode::INTERPRET_VALUE_ERROR);
        }

        for ($i = 0; $i < $this->value; $i++) {
            $block();
        }

        return $block();
    }
}