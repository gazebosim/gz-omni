import os
import sys
import argparse

import packmanapi
import repoman

DEPS = {
    "nvfilecopy": {
        "version": "1.4",
        "link_path_host": "nvtools_nvfilecopy",
    }
}


if __name__ == "__main__" or __name__ == "__mp_main__":
    repo_folders = repoman.api.get_repo_paths()
    deps_folders = repoman.api.fetch_deps(DEPS, None, repo_folders["host_deps"])

    sys.path.append(deps_folders["nvfilecopy"])
    import nvfilecopy

    parser = argparse.ArgumentParser()
    parser.add_argument('-p', '--platform-target', dest='platform_target', required=True)
    options, _ = parser.parse_known_args()
    platform_target = repoman.api.validate_platform(
        "target",
        options.platform_target,
        ["windows-x86_64", "linux-x86_64", "linux-aarch64"]
    )
    nvfilecopy.process_json_file(sys.argv[len(sys.argv) - 1], platform_target)
