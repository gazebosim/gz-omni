#!/bin/bash

set -e

SCRIPT_DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

echo Running script in ${SCRIPT_DIR}
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${SCRIPT_DIR}/_build/linux-x86_64/debug"

pushd $SCRIPT_DIR > /dev/null
gdb ./_build/linux-x86_64/debug/ignition-omniverse1 "$@"
popd > /dev/null
