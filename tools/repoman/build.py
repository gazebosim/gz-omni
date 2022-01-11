import os
import sys
import argparse

import repoman
import packmanapi


DEPS = {
    "nvtools_build": {
        "version": "0.3.2",
        "link_path_host": "nvtools_build",
    }
}


def run_command():
    platform_host = repoman.api.get_and_validate_host_platform(["windows-x86_64", "linux-x86_64"])
    repo_folders = repoman.api.get_repo_paths()
    repoman.api.fetch_deps(DEPS, platform_host, repo_folders["host_deps"])

    BUILD_SCRIPT = os.path.join(repo_folders["host_deps"], "nvtools_build", "build.py")

    # Fetch the asset dependencies
    parser = argparse.ArgumentParser()
    parser.add_argument('-p', '--platform-target',
                        dest='platform_target', required=False)
    options, _ = parser.parse_known_args()

    # Checking if platform was passed
    # We cannot use argparse's default, as we also need to set up the command line argument
    # if it wasn't supplied. It is possible to also check for the host platform, if we want to
    # make different default behavior when building on windows.
    if not repoman.api.has_options_arg(options, 'platform_target'):
        options.platform_target = 'linux-x86_64'
        sys.argv.extend(["--platform-target", options.platform_target])

    # We need the host-deps before we can run MSBuild
    packmanapi.pull(os.path.join(repo_folders["root"], repo_folders["host_deps_xml"]), platform=platform_host)

    # Construct arguments for the underlying script
    script_argv = sys.argv[1:]
    script_argv.extend(["--root", repo_folders["root"]])
    script_argv.extend(["--deps-host", repo_folders["host_deps_xml"]])
    script_argv.extend(["--deps-target", repo_folders["target_deps_xml"]])
    if platform_host == "windows-x86_64":
        script_argv.extend(["--premake-tool", os.path.join(repo_folders["host_deps"], "premake", "premake5.exe")])
        # Look for different MSBuild versions
        ms_build_path = ""
        ms_build_locations = [
            r"buildtools\MSBuild\15.0\Bin\MSBuild.exe",
            r"buildtools\MSBuild\Current\Bin\MSBuild.exe",
        ]
        for ms_build_location in ms_build_locations:
            print("Checking if MSBuild.exe located here: " + os.path.join(repo_folders["host_deps"], ms_build_location))
            if os.path.exists(os.path.join(repo_folders["host_deps"], ms_build_location)):
                ms_build_path = os.path.join(repo_folders["host_deps"], ms_build_location)
                break
        print("Building using this MSBuild: " + ms_build_path)
        script_argv.extend(["--msbuild-tool", ms_build_path])
        script_argv.extend(["--vs-version", "vs2019"])
        script_argv.extend(["--sln", os.path.join(repo_folders["compiler"], r"vs2019\Samples.sln")])
    elif platform_host == "linux-x86_64":
        script_argv.extend(["--premake-tool", os.path.join(repo_folders["host_deps"], "premake", "premake5")])

    # Execute module script and set globals
    repoman.api.run_script_with_custom_args(BUILD_SCRIPT, script_argv)


if __name__ == "__main__" or __name__ == "__mp_main__":
    run_command()
