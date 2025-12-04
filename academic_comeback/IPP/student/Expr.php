<?php

namespace IPP\Student;
use IPP\Student\Exception;
use IPP\Core\ReturnCode;
use IPP\Student\ClassManager;

class Expr
{
    /**
     * @var \DOMElement $xml
     */
    private $xml;
    /**
     * @var MyObjectInstance $result
     */
    private $result;
    /**
     * @var array<MyObjectInstance> $memory
     */
    private $memory;
    /**
     * @var MyBlockInstance $blockInstance
     */
    private $blockInstance;
    /**
     * @var mixed $classInstance
     */
    private $classInstance;

    /**
     * @param \DOMElement $xml
     * @param MyBlockInstance $blockInstance
     * @param MyObjectInstance $classInstance
     */
    public function __construct($xml, $blockInstance, $classInstance)
    {
        $this->xml = $xml;
        $this->memory = $blockInstance->getVariables();
        $this->blockInstance = $blockInstance;
        $this->classInstance = $classInstance;
    }

    /**
     * @return MyObjectInstance
     * @throws Exception
     */
    public function evaluate()
    {
        foreach ($this->xml->childNodes as $exprChild) {
            if (!($exprChild instanceof \DOMElement)) {
                continue;
            }
            switch ($exprChild->nodeName) {
                case 'literal':
                    $class = $exprChild->getAttribute('class');
                    $value = $exprChild->getAttribute('value');
                    if ($class =='class') {
                        $class = $value;
                        $value = null;
                    }
                    switch ($class) {
                        case 'Integer':
                            $this->result = new MyInt($value);
                            break;
                        case 'String':
                            $this->result = new MyString($value);
                            break;
                        case 'Nil':
                            $this->result = MyNil::getInstance();
                            break;
                        case 'True':
                            $this->result = MyTrue::getInstance();
                            break;
                        case 'False':
                            $this->result = MyFalse::getInstance();
                            break;
                        case 'Object':
                            $this->result = new MyObjectInstance();
                            break;
                        default:
                            $classTemplate = ClassManager::getClass($class);
                            if ($classTemplate == null)
                                throw new Exception("Unknown class type: $class", ReturnCode::INTERPRET_TYPE_ERROR);
                            // echo "Creating MyObjectInstance in expr with class: $class with value: $value\n";
                            $res = new MyObjectInstance($classTemplate, $value);
                            $res->setValue($value);
                            return $this->result = $res;
                    }
                    break;
                case 'send':
                    $subExpr = null;
                    $arg = null;
                    $class = null;
                    $selector = $exprChild->getAttribute('selector');
                    if ($selector == 'ifTrue:ifFalse:') {
                        $args = [];
                        foreach ($exprChild->childNodes as $selectorChild) {
                            if (!($selectorChild instanceof \DOMElement)) {
                                continue;
                            }
                            switch ($selectorChild->nodeName) {
                                case 'expr':
                                    $subExpr = new Expr($selectorChild, $this->blockInstance, $this->classInstance);
                                    break;
                                case 'arg':
                                    $blockElement = $selectorChild->getElementsByTagName('block')->item(0);
                                    $blockdeff = new Block($blockElement);
                                    $args[] = new MyBlockInstance($blockdeff, $this->classInstance);
                                    break;
                            }
                        }
                        $if = new MyIfTrueIfFalse($args[0], $args[1]);
                        $this->result = $if->receiveMessage(null, $subExpr);
                    } elseif ($selector == 'and:') {
                        $args = [];
                        foreach ($exprChild->childNodes as $selectorChild) {
                            if (!($selectorChild instanceof \DOMElement)) {
                                continue;
                            }
                            switch ($selectorChild->nodeName) {
                                case 'expr':
                                    $subExpr = new Expr($selectorChild, $this->blockInstance, $this->classInstance);
                                    break;
                                case 'arg':
                                    $blockElement = $selectorChild->getElementsByTagName('block')->item(0);
                                    if ($blockElement !== null) {
                                        $blockdeff = new Block($blockElement);
                                        $arg = new MyBlockInstance($blockdeff, $this->classInstance);
                                    } else {
                                        $argElement = $selectorChild->getElementsByTagName('expr')->item(0);
                                        $argExpr = new Expr($argElement, $this->blockInstance, $this->classInstance);
                                        $arg = $argExpr->evaluate();
                                    }
                                    break;
                            }
                        }
                        $and = new MyAnd($arg);
                        $res = $and->receiveMessage(null, $subExpr);
                        $this->result = $res;
                    } elseif ($selector == 'or:') {
                        $args = [];
                        foreach ($exprChild->childNodes as $selectorChild) {
                            if (!($selectorChild instanceof \DOMElement)) {
                                continue;
                            }
                            switch ($selectorChild->nodeName) {
                                case 'expr':
                                    $subExpr = new Expr($selectorChild, $this->blockInstance, $this->classInstance);
                                    break;
                                case 'arg':
                                    $blockElement = $selectorChild->getElementsByTagName('block')->item(0);
                                    if ($blockElement !== null) {
                                        $blockdeff = new Block($blockElement);
                                        $arg = new MyBlockInstance($blockdeff, $this->classInstance);
                                    } else {
                                        $argElement = $selectorChild->getElementsByTagName('expr')->item(0);
                                        $argExpr = new Expr($argElement, $this->blockInstance, $this->classInstance);
                                        $arg = $argExpr->evaluate();
                                    }
                                    break;
                            }
                        }
                        $or = new MyOr($arg);
                        $this->result = $or->receiveMessage(null, $subExpr);
                    } elseif ($selector == 'from:') {
                        foreach ($exprChild->childNodes as $selectorChild) {
                            if (!($selectorChild instanceof \DOMElement)) {
                                continue;
                            }
                            switch ($selectorChild->nodeName) {
                                case 'expr':
                                    $expr = new Expr($selectorChild, $this->blockInstance, $this->classInstance);
                                    $class = $expr->evaluate();
                                    break;
                                case 'arg':
                                    $exprElement = $selectorChild->getElementsByTagName('expr')->item(0);
                                    $arg = new Expr($exprElement, $this->blockInstance, $this->classInstance);
                                    $arg = $arg->evaluate();
                                    // echo get_class($arg) . "\n";
                                    // echo "Arg: " . $arg->getValue() . "\n";
                                    break;
                            }
                        }
                        // print_r($class);
                        // echo "Class: " . get_class($class) . " args: " . $arg->getValue() . "\n";
                        // print_r($class->getClass());
                        // echo "XD\n";
                        $this->result = $class->receiveMessage('from', [$class->getClass(), $arg]);
                    } else {
                        $args = [];
                        foreach ($exprChild->childNodes as $selectorChild) {
                            if (!($selectorChild instanceof \DOMElement)) {
                                continue;
                            }
                            switch ($selectorChild->nodeName) {
                                case 'expr':
                                    $subExpr = new Expr($selectorChild, $this->blockInstance, $this->classInstance);
                                    $this->result = $subExpr->evaluate();
                                    // echo $this->result->getClass()->getValue() . "  value\n";
                                    break;
                                case 'arg':
                                    $exprElement = $selectorChild->getElementsByTagName('expr')->item(0);
                                    $argExpr = new Expr($exprElement, $this->blockInstance, $this->classInstance);
                                    $args[] = $argExpr->evaluate();
                                    break;
                            }
                        }
                    // echo "Selector: $selector\n";
                    if ($this->result == null) {
                        throw new Exception("Selector $selector not found in class.", ReturnCode::INTERPRET_TYPE_ERROR);
                    }
                    $this->result = $this->result->receiveMessage($selector, $args);
                    }
                    break;
                case 'var':
                    $varName = $exprChild->getAttribute('name');
                    if ($varName == 'self') {
                        $this->result = $this->classInstance;
                    } else if ($varName == 'super') {
                        $this->result = $this->classInstance->getClass()->getParent();
                    } else if (isset($this->memory[$varName])) {
                        // echo "Getting variable: $varName ";
                        // echo "Value: " . $this->memory[$varName]->getValue() . "\n";
                        // echo "Type: " . get_class($this->memory[$varName]) . "\n";
                        return $this->memory[$varName];
                    } else {
                        throw new Exception("Variable $varName not found in memory.", ReturnCode::INTERPRET_TYPE_ERROR);
                    }
                    break;
            }
        }

        return $this->result;
    }
}