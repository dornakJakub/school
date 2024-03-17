#!/bin/sh

# Turn on POSIX mode
export POSIXLY_CORRECT=1

help() {
    echo "Usage: $0 [-h|--help] [FILTR] [PŘÍKAZ] UŽIVATEL LOG [LOG2 [...]]"
    echo "Options:"
    echo "  -h, --help : Display help"
    exit 1
}

#Argument parsing
while getopts ":h-:" opt; do
    case $opt in
        -)
            case "${OPTARG}" in
                help)
                    help
                    ;;
                *)
                    echo "Invalid option: --$OPTARG" >&2
                    help
                    ;;
            esac
            ;;
        h)
            help
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            help
            ;;
    esac
done