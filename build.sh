#!/bin/bash

set -e

rebuild () {
    "$SCRIPT_DIR/tools/packman/python.sh" "$SCRIPT_DIR/tools/repoman/clean.py" $@ || exit 1
}

clean () {
    "$SCRIPT_DIR/tools/packman/python.sh" "$SCRIPT_DIR/tools/repoman/clean.py"
    exit $?
}

SCRIPT_DIR=$(dirname ${BASH_SOURCE})

POSITIONAL=()
while [[ $# -gt 0 ]]
do
    case $1 in
        -c|--clean)
        clean
        ;;
        -x|--rebuild)
        rebuild
        shift
        ;;
        *)
        POSITIONAL+=("$1")
        shift
        ;;
    esac
done

set -- "${POSITIONAL[@]}" # restore positional parameters


./prebuild.sh "$@" || exit $?

ARGS_FLATTENED="$@"
source "$SCRIPT_DIR/tools/packman/python.sh" "$SCRIPT_DIR/tools/repoman/build.py" $ARGS_FLATTENED || exit $?
