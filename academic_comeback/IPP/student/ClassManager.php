<?php

namespace IPP\Student;

use IPP\Student\ClassTemplate;
use DOMNodeList;
use DOMElement;

class ClassManager
{
    /**
     * @var array<ClassTemplate>
     */
    private static $classes = [];

    /**
     * @param DOMNodeList<DOMElement> $sourceXML
     * @return void
     */
    public static function buildClasses($sourceXML)
    {
        foreach ($sourceXML as $classXML) {
            $className = $classXML->getAttribute('name');
            $methods = [];

            $methodsXML = $classXML->getElementsByTagName('method');
            foreach ($methodsXML as $methodXML) {
                $methodName = $methodXML->getAttribute('selector');

                $body = $methodXML->getElementsByTagName('block')->item(0);
                // echo $methodXML->ownerDocument->saveXML($body);
                $methods[$methodName] = new Method($methodName, $body);
            }

            self::$classes[$className] = new ClassTemplate($className, $methods);
        }
    }

    /**
     * @param string $name
     * @return ClassTemplate|null
     */
    public static function getClass($name)
    {
        if (array_key_exists($name, self::$classes)) {
            return self::$classes[$name];
        } else {
            return null;
        }
    }
}