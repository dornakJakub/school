<?php

namespace IPP\Student;

use IPP\Core\AbstractInterpreter;
use IPP\Core\Exception\XMLException;
use IPP\Student\Exception;
use IPP\Core\ReturnCode;

class Interpreter extends AbstractInterpreter
{
    public function execute(): int
    {
        // TODO: Start your code here
        // Check IPP\Core\AbstractInterpreter for predefined I/O objects:
        $dom = $this->source->getDOMDocument();
        // $val = $this->input->readString();
        // $this->stdout->writeString("stdout");
        // $this->stderr->writeString("stderr");
        // $GLOBALS['stdin'] = $this->input;
        // $GLOBALS['stdout'] = $this->stdout;

        IOManager::init($this->stdout, $this->input);
        
        $program = $dom->getElementsByTagName("program")->item(0);
        if ($program === null) {
            throw new Exception("Program element not found");
        }

        $classes = $dom->getElementsByTagName('class');
        ClassManager::buildClasses($classes);

        $main = ClassManager::getClass('Main');
        if ($main === null) {
            throw new Exception("Main class not found", ReturnCode::INTERPRET_TYPE_ERROR);
        }
        $run = $main->getMethod('run');
        if ($run === null) {
            throw new Exception("Method run not found", ReturnCode::INTERPRET_TYPE_ERROR);
        }
        $mainInstance = new MyObjectInstance($main);
        $run->execute($mainInstance);

        return 0;
    }
}
