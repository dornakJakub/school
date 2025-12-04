<?php

namespace IPP\Student;

class MyIfTrueIfFalse extends MyObjectInstance
{
    /**
     * @var MyBlockInstance $trueBlock
     */
    private $trueBlock;

    /**
     * @var MyBlockInstance $falseBlock
     */
    private $falseBlock;

    /**
     * @param MyBlockInstance $trueBlock
     * @param MyBlockInstance $falseBlock
     */
    public function __construct($trueBlock, $falseBlock)
    {
        $this->trueBlock = $trueBlock;
        $this->falseBlock = $falseBlock;
    }

    /**
     * @param Expr $args
     */
    public function receiveMessage($message, $args)
    {
        // echo "IfTrueIfFalse: receiveMessage $message\n";
        if ($args->evaluate() instanceof MyTrue) {
            // echo "IfTrueIfFalse: true block\n";
            return $this->trueBlock->execute();
        } else {
            // echo "IfTrueIfFalse: false block\n";
            return $this->falseBlock->execute();
        }
    }
}