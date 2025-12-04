<?php

namespace IPP\Student;

use IPP\Core\Interface\OutputWriter;
use IPP\Core\Interface\InputReader;

class IOManager
{
    /**
     * @var OutputWriter $stdout
     */
    private static $stdout;
    /**
     * @var InputReader $stdin
     */
    private static $stdin;

    /**
     * @param OutputWriter $stdout
     * @param InputReader $stdin
     */
    public static function init($stdout, $stdin) :void
    {
        self::$stdout = $stdout;
        self::$stdin = $stdin;
    }

    /**
     * @return OutputWriter
     */
    public static function getStdout()
    {
        return self::$stdout;
    }

    /**
     * @return InputReader
     */
    public static function getStdin()
    {
        return self::$stdin;
    }
}