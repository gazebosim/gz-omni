#!/bin/bash

set -e

SCRIPT_DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

echo Running script in ${SCRIPT_DIR}

pushd $SCRIPT_DIR > /dev/null

export USD_LIB_DIR=${SCRIPT_DIR}/_build/target-deps/nv_usd/release/lib
export OMNI_CLIENT_DIR=${SCRIPT_DIR}/_build/target-deps/omni_client_library/release
export PYTHON=${SCRIPT_DIR}/_build/target-deps/python/python

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${USD_LIB_DIR}:${OMNI_CLIENT_DIR}
export PYTHONPATH=${USD_LIB_DIR}/python:${OMNI_CLIENT_DIR}/bindings-python

if [ ! -f ${PYTHON} ]; then
    echo "echo Python, USD, and Omniverse Client libraries are missing.  Run ./prebuild.sh to retrieve them."
    popd
    exit
fi

${PYTHON} ./source/pyHelloWorld/helloWorld.py "$@"
popd > /dev/null
