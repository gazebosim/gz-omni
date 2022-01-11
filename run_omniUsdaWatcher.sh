#!/bin/bash

set -e

SCRIPT_DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

echo Running script in ${SCRIPT_DIR}
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${SCRIPT_DIR}/_build/linux-x86_64/release"

pushd $SCRIPT_DIR > /dev/null
./_build/linux-x86_64/release/omniUsdaWatcher "$@"
popd > /dev/null
