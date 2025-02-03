#!/bin/bash

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export CARB_APP_PATH=${SCRIPT_DIR}/_build/linux-x86_64/release

export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${CARB_APP_PATH}:/usr/local/google/home/shameek/omni-workspace/install/lib"

export GZ_FILE_PATH=/var/apps/gzserver/meshes

echo Running script in ${SCRIPT_DIR}
pushd "$SCRIPT_DIR" > /dev/null
"${CARB_APP_PATH}/gazebo-omniverse1" "$@"
popd > /dev/null
