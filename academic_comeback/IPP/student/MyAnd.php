<?php

namespace IPP\Student;

class MyAnd extends MyObjectInstance
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
     * @param Expr $args
     */
    public function receiveMessage($message, $args)
    {
        if ($args->evaluate() instanceof MyTrue) {
            if ($this->block instanceof MyBlockInstance) {
                $res = $this->block->execute();
            } else {
                if ($this->block->getValue() instanceof MyTrue)
                    return MyTrue::getInstance();
                return MyFalse::getInstance();
            }
            return $res;
        } else {
            return MyFalse::getInstance();
        }
    }
}