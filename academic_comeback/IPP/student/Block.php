<?php

namespace IPP\Student;

use DOMElement;

class Block
{
    /**
     * @var array<int, DOMElement> $commands
     */
    private $commands = [];

    /**
     * @param DOMElement $block
     */
    public function __construct($block)
    {
        $assigns = [];
        foreach ($block->childNodes as $child) {
            if ($child instanceof DOMElement && $child->tagName === 'assign') {
                $assigns[] = $child;
            }
        }

        foreach ($assigns as $assign) {
            $order = (int)$assign->getAttribute('order');
            $this->commands[] = ['order' => $order, 'node' => $assign];
        }

        if ($this->commands === null) {
            $this->commands = [];
        }

        usort($this->commands, function ($a, $b) {
            return $a['order'] <=> $b['order'];
        });

        $parent = $block->parentNode;
        while ($parent !== null && $parent->nodeName !== 'class') {
            $parent = $parent->parentNode;
        }
    }

    /**
     * @return MyObjectInstance
     * @param MyBlockInstance $blockInstance
     * @param MyObjectInstance $classInstance
     */
    public function execute($blockInstance, $classInstance)
    {
        $res = null;
        $varName = null;
        foreach ($this->commands as $command) {
            $assign = $command['node']->childNodes;
            foreach ($assign as $child) {
                if (!($child instanceof DOMElement)) {
                    continue;
                }
                switch ($child->nodeName) {
                    case 'var':
                        $varName = $child->getAttribute('name');
                        break;
                    case 'expr':
                        $expr = new Expr($child, $blockInstance, $classInstance);
                        $res = $expr->evaluate();
                        // echo "Evaluated expression for variable: " . $varName . "\n";
                        // print_r($res);
                        $blockInstance->setVariable($varName, $res);
                        break;
                }
            }
        }

        return $res;
    }
}