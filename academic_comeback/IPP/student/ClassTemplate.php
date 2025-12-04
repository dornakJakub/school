<?php

namespace IPP\Student;
use IPP\Core\ReturnCode;
use IPP\Student\Exception;

/*
* Base abstract class for object instances
*/

class ClassTemplate
{   
    /**
     * @var string $name
     */
    protected $name;
    /**
     * @var array<Method> $methods
     */
    protected $methods;

    /**
     * @param string $name
     * @param array<Method> $methods
     */
    public function __construct($name, $methods = [])
    {
        $this->name = $name;
        $this->methods = $methods;
    }

    /**
     * @param string $name
     * @return Method|null
     */
    public function getMethod($name) {
        if (!array_key_exists($name, $this->methods)) {
            return null;
        }
        return $this->methods[$name];
    }
}