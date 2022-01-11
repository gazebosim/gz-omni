import os
import sys

import packmanapi

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
REPO_ROOT_DIR = os.path.join(SCRIPT_DIR, "..", "..")
HOST_DEPS_PATH = os.path.join(REPO_ROOT_DIR, "_build", "host-deps")

repoman_link_path = os.path.abspath(os.path.join(HOST_DEPS_PATH, "nvtools_repoman"))

packmanapi.install("repo_repoman", package_version="0.1.1-beta2", link_path=repoman_link_path)

sys.path.append(repoman_link_path)
import api
