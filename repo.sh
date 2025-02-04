#!/bin/bash

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "$SCRIPT_DIR"

# gz-omni should be installed as part of a colcon workspace for it to build correctly.
WORKSPACE_INSTALL_DIR="../../install"
if [[ ! -d "$WORKSPACE_INSTALL_DIR" ]]; then
  echo "Failed to run script. Did you install gz-omni correctly in a colcon workspace?"
  exit 1
fi

# Set OMNI_REPO_ROOT early so `repo` bootstrapping can target the repository
# root when writing out Python dependencies.
export OMNI_REPO_ROOT="$( pwd -P )"

exec "tools/packman/python.sh" tools/repoman/repoman.py "$@"
