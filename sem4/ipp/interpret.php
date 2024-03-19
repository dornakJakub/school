<?php
#argument parsing
$options = getopt(null, ['source:', 'input:', 'help']);

if ($argc < 2) {
    exit(10);
}

//Check for --help argument
if (isset($options['help'])) {
    if ($argc > 2) {
        die("--help argument can't be mixed with other arguments\n");
    }
    echo "Help\n";
    exit;
}

if (!isset($options['source']) && !isset($options['input'])) {
    die("At least one of --source or --input must be provided.\n");
}

//Set files
if (isset($options['source'])) {
    $sourceFile = $options['source'];
    if (!file_exists($sourceFile)) {
        exit(12);
    }
    echo "Processing source file: $sourceFile\n";
}

if (isset($options['input'])) {
    $inputFile = $options['input'];
    if (!file_exists($inputFile)) {
        exit(11);
    }
    echo "Processing input file: $inputFile\n";
}
?>