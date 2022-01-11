import os
import sys
import argparse
import subprocess
import json
from xml.etree import ElementTree

import repoman
import packmanapi


DEPS = {
    "nvtools_build": {
        "version": "0.2.0",
        "link_path_host": "nvtools_build",
    }
}

'''
buildtools: C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools
vc: ../_build/host-deps/buildtools/VC/Tools/MSVC/14.27.29110
'''
def update_host_deps(host_deps_path:str, vs_path:str = "", msvc_ver:str = ""):
    # Preserving comments in the XML host_deps file
    # credit: https://stackoverflow.com/questions/33573807/faithfully-preserve-comments-in-parsed-xml
    class CommentedTreeBuilder(ElementTree.TreeBuilder):
        def comment(self, data):
            self.start(ElementTree.Comment, {})
            self.data(data)
            self.end(ElementTree.Comment)

    parser = ElementTree.XMLParser(target=CommentedTreeBuilder())
    # python 3.8 adds insert_comments
    #parser = ElementTree.XMLParser(target=ElementTree.TreeBuilder(insert_comments=True))

    vc_path = os.path.join("..", "_build", "host-deps", "buildtools", "VC", "Tools", "MSVC", msvc_ver)

    # Use winsdk.bat from Visual Studio tools to find the Windows SDK
    windows_sdk_dir = ""
    windows_sdk_ver = ""
    windows_sdk_bin_dir = ""
    windows_sdk_lib_dir = ""
    windows_sdk_include_dir = ""

    winsdk_bat_path = os.path.join(vs_path, "Common7", "Tools", "vsdevcmd", "core", "winsdk.bat")
    if os.path.exists(winsdk_bat_path):
        # We have a batch wrapper that calls the winsdk.bat file and emits the important env vars to be processed
        script_path = os.path.split(os.path.abspath(__file__))[0]
        cmd_line = []
        cmd_line.append(os.path.join(script_path, "print_winsdk_env_vars.bat"))
        cmd_line.append(winsdk_bat_path)
        completed = subprocess.run(cmd_line, capture_output=True)
        for line in completed.stdout.decode().splitlines():
            if "WindowsSDKDir" in line:
                windows_sdk_dir = line.split("=")[1].rstrip("\\")
            elif "WindowsSdkVersion" in line:
                windows_sdk_ver = line.split("=")[1].rstrip("\\")

        if os.path.exists(windows_sdk_dir):
            windows_sdk_bin_dir = os.path.join(windows_sdk_dir, "bin", windows_sdk_ver)
            windows_sdk_include_dir = os.path.join(windows_sdk_dir, "include", windows_sdk_ver)
            windows_sdk_lib_dir = os.path.join(windows_sdk_dir, "lib", windows_sdk_ver)

    # Read the XML tree from the host_deps file
    tree = ElementTree.parse(host_deps_path, parser)
    root = tree.getroot()

    # Replace the builtools and vc paths
    find_replace_dict = {
        "buildtools": vs_path,
        "vc": vc_path,
        "winsdk": windows_sdk_dir,
        "winsdk_bin": windows_sdk_bin_dir,
        "winsdk_include": windows_sdk_include_dir,
        "winsdk_lib": windows_sdk_lib_dir,
    }
    for dependency in root.findall("dependency"):
        for find_key in find_replace_dict.keys():
            if "name" in dependency.attrib.keys() and find_key == dependency.attrib["name"] and find_replace_dict[find_key] != "":
                for source in dependency.iter("source"):
                    source.attrib["path"] = find_replace_dict[find_key]
                    print("Updating <%s> attribute with <%s>" % (dependency.attrib["name"],source.attrib["path"]))
                    
    tree.write(host_deps_path)


'''
find_vs will search through the display names of the installed Visual Studio versions and 
return the installation path for the first one that matches the input string provided

current display names:
* Visual Studio Community 2019
* Visual Studio Professional 2019
* Visual Studio Professional 2017
'''
def find_vs(search_str:str, listall:bool = False) -> str:
    program_files = os.getenv("ProgramFiles(x86)")
    if not program_files:
        print("ERROR: No Program Files (x86) directory found")
        return None
    vswhere_path = os.path.join(program_files, "Microsoft Visual Studio", "Installer", "vswhere.exe")
    if not os.path.exists(vswhere_path):
        print("ERROR: vswhere.exe is not found here, so no Visual Studio installations found: " + vswhere_path)
        return None

    # Run vswhere with a json-formatted output
    cmd_line = list()
    cmd_line.append(vswhere_path)
    cmd_line.append("-products")
    cmd_line.append("*")
    cmd_line.append("-format")
    cmd_line.append("json")

    completed = subprocess.run(cmd_line, capture_output=True)
    version_strings = completed.stdout.decode()
    version_json = json.loads(version_strings)

    # Find the requested version using the displayName attribute
    last_version = None
    for vs_version in version_json:
        if listall:
            print(vs_version["displayName"])
            last_version = vs_version["installationPath"]
        elif search_str in vs_version["displayName"]:
            return vs_version["installationPath"]
    
    return last_version


'''
find_msvc_ver will list the first MSVC version found in a Visual Studio installation
vs_install_path = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community"
returns something like "14.25.28610"
'''
def find_msvc_ver(vs_install_path:str) -> str:
    # return first folder found in "VC/Tools/MSVC"
    msvc_folder = os.path.join(vs_install_path, "VC", "Tools", "MSVC")
    if not os.path.exists(msvc_folder):
        print("ERROR: No MSVC folder found at " + msvc_folder)
        return None

    msvc_vers = os.listdir(msvc_folder)
    if len(msvc_vers) > 0:
        return msvc_vers[0]
    else:
        print("ERROR: No MSVC folder found at " + msvc_folder)
        return None

def run_command():
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', 
        '--visual-studio-version',
        default='2019',
        dest='vs_ver',
        help='Different Visual Studio installation \"displayNames\" will be searched with this substring',
        required=False)
    parser.add_argument('-l', 
        '--list-all',
        dest='list_all', 
        action='store_true',
        help="Enable this to simply just list all Visual Studio installations rather than updating the host_deps file",
        required=False)
    parser.add_argument('-d', 
        '--host-deps-path',
        dest='host_deps_path', 
        help="The path to the host_deps.packman.xml file",
        required=False)
    args, _ = parser.parse_known_args()

    if not args.list_all:
        print("Searching for an install of Visual Studio <%s>" % (args.vs_ver))

    vs_path = find_vs(args.vs_ver, args.list_all)
    if not vs_path:
        print("ERROR: No Visual Studio Installation Found")
        exit(1)

    if not args.list_all and vs_path:
        print("VS " + args.vs_ver + " found in: " + vs_path)

        msvc_version = find_msvc_ver(vs_path)
        if msvc_version:
            print("VS " + args.vs_ver + " MSVC ver: " + msvc_version)

        if args.host_deps_path and vs_path and msvc_version:
            update_host_deps(args.host_deps_path, vs_path = vs_path, msvc_ver = msvc_version)
            print("Update host dependencies file: " + args.host_deps_path)

if __name__ == "__main__" or __name__ == "__mp_main__":
    run_command()
