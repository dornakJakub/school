<?php

namespace IPP\Student;
use DOMElement;

class Method
{
    /**
     * @var string
     */
    private $name;
    /**
     * @var Block
     */
    private $block;

    /**
     * @param string $name
     * @param DOMElement $block
     */
    public function __construct($name, $block)
    {
        $this->name = $name;
        $this->block = new Block($block);
    }

    /**
     * @return string
     */
    public function getName()
    {
        return $this->name;
    }

    /**
     * @return Block
     */
    public function getBody()
    {
        return $this->block;
    }

    /**
     * @param MyObjectInstance $classInstance
     * @return void
     */
    public function execute($classInstance)
    {
        $blockInstance = new MyBlockInstance($this->block, $classInstance);
        $blockInstance->execute();
    }
}