#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 03/2025
#    LICENSE  : CeCILL-C
#    FILE     : dev/update_version.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
import os
import subprocess
from update_file_headers import HeaderPatcher
from termcolor import cprint

############################################################
class UpdateVersion:
    def __init__(self):
         # set defaults
         self.old_version = "X.X.X-dev"
         self.old_short_version = "X.X.X"

         # check
         self.check_is_source_root_dir()

         # load
         self.extract_old_version()

    def extract_old_version(self):
        self.old_version = subprocess.check_output('cat configure | grep VERSION | xargs echo | cut -d " " -f 4', shell=True).decode("utf-8").replace("\n", "")
        self.old_short_version = self.old_version.replace("-dev", "").replace("-beta", "").replace("\n", "")
        print(f"[version] Old version is {self.old_version}, short is {self.old_short_version}")

    def check_is_source_root_dir(self):
        print("[check] Source dir")
        if not os.path.exists("./dev/update-version.sh"):
            raise Exception("Caution, you must call this script from the project root directory !")

    def rebuild_manpages(self):
        print("[manpages] Rebuild ...")
        subprocess.run("make", cwd="./src/manpages")
        print("[manpages] Rebuild done")

    def update_file_headers_version_and_authors(self, target_version):
        print("[headers] Update version & authors in file headers ...")
        print("------------------------------------------------------")
        
         # load
        patcher = HeaderPatcher("./dev/update_file_headers.json")
        patcher.patch_all_git_files(".")

        print("------------------------------------------------------")
        print("[headers] Update version & authors in file headers done")

############################################################
def malt_update_version(target_version, extra):
    print(f"[version] Set version {target_version}{extra}")
    updater = UpdateVersion()
    updater.check_is_source_root_dir()
    updater.extract_old_version()
    updater.update_file_headers_version_and_authors(target_version)
    updater.rebuild_manpages()
