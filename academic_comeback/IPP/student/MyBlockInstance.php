<?php

namespace IPP\Student;

class MyBlockInstance extends MyObjectInstance
{
    /**
     * @var Block
     */
    private $blockDef;
    /**
     * @var array<MyObjectInstance> $variables
     */
    private $variables = [];
    /**
     * @var MyObjectInstance
     */
    private $objectInstance;

    /**
     * @param Block $blockDef
     * @param MyObjectInstance $objectInstance
     */
    public function __construct($blockDef, $objectInstance)
    {
        $this->blockDef = $blockDef;
        $this->objectInstance = $objectInstance;
    }

    /**
     * @return MyObjectInstance
     */
    public function execute()
    {
        return $this->blockDef->execute($this, $this->objectInstance);
    }

    /**
     * @return array<MyObjectInstance>
     */
    public function getVariables()
    {
        return $this->variables;
    }

    /**
     * @param string $name
     * @param MyObjectInstance $value
     * @return void
     */
    public function setVariable($name, $value)
    {
        $this->variables[$name] = $value;
    }
}