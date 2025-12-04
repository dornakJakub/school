<?php

namespace IPP\Student;

use IPP\Core\ReturnCode;

class MyObjectInstance
{
    /**
     * @var ClassTemplate $class
     */
    private $class;
    /**
     * @var mixed $value
     */
    private $value;
    /**
     * @var array<MyObjectInstance> $attributes
     */
    private $attributes = [];

    /**
     * @param ClassTemplate $class
     * @param mixed $value
     */
    public function __construct($class = null, $value = null)
    {
        $this->class = $class;
        $this->value = $value;
    }

    /**
     * @param string|null $message
     * @param array<MyObjectInstance>|Object $args
     * @return MyObjectInstance
     * @throws Exception
     */
    public function receiveMessage($message, $args)
    {
        // echo "Calling method: $methodName on: $this->name value: " . $this->class->getValue() . " of type: " . get_class($this->class) ."\n";
        $message = str_replace(':', '', $message);
        $userMethod = null;
        if ($this->class !== null) {
            // echo "\nmessage: " . $message . "\n";
            $userMethod = $this->class->getMethod($message);
        }
        if ($userMethod !== null) {
            return $userMethod($args);
        }
        else if (method_exists($this, $message)) {
            if ($message == 'new')
                $args[0] = $this->class;
            return $this->$message($args);
        }
        else {
            if (count($args) == 0) {
                if (array_key_exists($message, $this->attributes)) {
                    return $this->attributes[$message];
                }

                throw new Exception("Method $message not found", ReturnCode::INTERPRET_DNU_ERROR);
            }
            $this->attributes[$message] = $args[0];
            return $this->attributes[$message];
        }
    }

    /**
     * @return ClassTemplate|MyObjectInstance
     */
    public function getClass()
    {
        // echo "getClass: " . "\n";
        if ($this->class === null) {
            return $this;
        }
        return $this->class;
    }

    /**
     * @param mixed $value
     * @return void
     */
    public function setValue($value)
    {
        $this->value = $value;
    }

    /**
     * @return mixed
     */
    public function getValue()
    {
        return $this->value;
    }

    /**
     * @param MyObjectInstance $object
     * @return MyBool
     */
    public function identicalTo($object)
    {
        if ($this === $object[0])
            return MyTrue::getInstance();
        return MyFalse::getInstance();
    }

    /**
     * @param MyObjectInstance $object
     * @return MyBool
     */
    public function equalTo($object)
    {
        if (get_object_vars($this) === []) {
            return $this->identicalTo($object[0]);
        }

        // echo "Comparing: " . json_encode($this) . "\n";
        if (get_class($this) !== get_class($object[0]))
            return MyFalse::getInstance();
        if (get_object_vars($this) === get_object_vars($object[0]))
            return MyTrue::getInstance();
        return MyFalse::getInstance();
    }

    /**
     * @return MyObjectInstance
     */
    public function asString()
    {
        return new MyString();
    }

    /**
     * @return MyBool
     */
    public function isNumber()
    {
        return MyFalse::getInstance();
    }

    /**
     * @return MyBool
     */
    public function isString()
    {
        return MyFalse::getInstance();
    }

    /**
     * @return MyBool
     */
    public function isBlock()
    {
        return MyFalse::getInstance();
    }

    /**
     * @return MyBool
     */    
    public function isNil()
    {
        return MyFalse::getInstance();
    }

    /**
     * @param mixed $arg
     * @return MyObjectInstance
     */
    public static function new($arg)
    {
        return match (static::class) {
            MyNil::class => MyNil::getInstance(),
            MyTrue::class => MyTrue::getInstance(),
            MyFalse::class => MyFalse::getInstance(),
            MyString::class => new MyString($arg[0]),
            MyInt::class => new MyInt($arg[0]),
            MyObjectInstance::class => new MyObjectInstance($arg[0]),
            default => throw new \LogicException('Unsupported class: ' . static::class)
        };
    }

    /**
     * @param array<MyObjectInstance|ClassTemplate> $arg
     * @return static
     */
    public static function from($arg): static
    {
        $class = new \ReflectionClass(static::class);

        if ($arg[0] instanceof ClassTemplate) {
            $res = $class->newInstance($arg[0]);
        } else {
            $res = $class->newInstance($arg[0]->getClass());
        }
    
        $res->setValue($arg[1]->getValue());
        return $res;
    }
}