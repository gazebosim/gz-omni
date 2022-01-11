import os
import sys
import packmanapi

packagemaker_path = packmanapi.install("packagemaker", package_version="4.0.0-rc9", link_path='_packages/packagemaker')
sys.path.append('_packages/packagemaker')

import packagemaker


def main():
    pkg = packagemaker.PackageDesc()
    pkg.version = os.getenv('BUILD_NUMBER', '0')
    pkg.output_folder = '_unsignedpackages'
    pkg.name = 'samples'
    pkg.files = [
        ('_build/windows-x86_64/release/*.exe'),
        ('_build/windows-x86_64/release/*.dll'),
    ]

    packagemaker.package(pkg)


if __name__ == '__main__' or __name__ == '__mp_main__':
    main()
