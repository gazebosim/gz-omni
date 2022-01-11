import os
import sys
import platform


def clean():
    folders = [
        '_build',
        '_compiler',
        '_builtpackages'
    ]
    for folder in folders:
        # having to do the platform check because its safer when you might be removing
        # folders with windows junctions.
        if os.path.exists(folder):
            print("Removing %s" % folder)
            if platform.system() == 'Windows':
                os.system("rmdir /q /s %s > nul 2>&1" % folder)
            else:
                os.system("rm -r -f %s > /dev/null 2>&1" % folder)
            if os.path.exists(folder):
                print("Warning: %s was not successfully removed, most probably due to a file lock on 1 or more of the files." % folder)


if __name__ == "__main__" or __name__ == "__mp_main__":
    clean()
