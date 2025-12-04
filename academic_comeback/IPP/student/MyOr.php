<?php

namespace IPP\Student;

class MyOr extends MyObjectInstance
{
    /**
     * @var MyBlockInstance|MyObjectInstance
     */
    private $block;

    /**
     * @param MyBlockInstance|MyObjectInstance $block
     */
    public function __construct($block)
    {
        $this->block = $block;
    }

    /**
     * @param null $message
     * @param Expr $args
     * @return MyObjectInstance
     */
    public function receiveMessage($message, $args)
    {
        if ($args->evaluate() instanceof MyFalse) {
            if ($this->block instanceof MyBlockInstance) {
                $res = $this->block->execute();
            } else {
                $res = $this->block->getValue();
            }
            return $res;
        } else {
            return MyTrue::getInstance();
        }
    }
}