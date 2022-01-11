#!/bin/bash

set -e

SCRIPT_DIR=$(dirname ${BASH_SOURCE})

# inject "-p linux-x86_64" as the default if no target platform is specified
for i in "$@"
do
    case $i in
        -p=*|--platform-target=*|-p|--platform-target)
        PLATFORM_TARGET_SET=1
        ;;
    esac
done

ARGS_ARRAY=("$@")
if [ -z "$PLATFORM_TARGET_SET" ]; then 
    ARGS_ARRAY+=("-p linux-x86_64") 
fi

ARGS_ARRAY_FLATTENED="${ARGS_ARRAY[@]}"
source "$SCRIPT_DIR/tools/packman/python.sh" "$SCRIPT_DIR/tools/repoman/build.py" --generateprojects-only ${ARGS_ARRAY_FLATTENED[@]} || exit $?
source "$SCRIPT_DIR/tools/packman/python.sh" "$SCRIPT_DIR/tools/repoman/filecopy.py" ${ARGS_ARRAY_FLATTENED[@]} "$SCRIPT_DIR/tools/buildscripts/pre_build_copies.json" || exit $?
