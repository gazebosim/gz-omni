#!/bin/bash

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export CARB_APP_PATH=${SCRIPT_DIR}/_build/linux-x86_64/debug

export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${CARB_APP_PATH}"

export GZ_FILE_PATH=/var/apps/gzserver/meshes

# gz-omni should be installed as part of a colcon workspace for it to launch correctly.
WORKSPACE_INSTALL_DIR="$SCRIPT_DIR/../../install"
if [[ ! -d "$WORKSPACE_INSTALL_DIR" ]]; then
  echo "Failed to run script. Did you install gz-omni correctly in a colcon workspace?"
  exit 1
fi
source "$WORKSPACE_INSTALL_DIR/setup.bash"

echo Running script in ${SCRIPT_DIR}
pushd "$SCRIPT_DIR" > /dev/null
gdb "${CARB_APP_PATH}/gazebo-omniverse1" "$@"
popd > /dev/null
