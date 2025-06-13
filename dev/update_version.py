#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.0
#    DATE     : 03/2025
#    LICENSE  : CeCILL-C
#    FILE     : dev/update_version.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
import os
import subprocess
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
        self.old_version = subprocess.check_output('cat configure | grep VERSION | xargs echo | cut -d " " -f 4', shell=True)
        self.old_short_version = self.old_version.replace("-dev")
        print(f"Old version is {self.old_version}, short is {self.old_short_version}")

    def check_is_source_root_dir(self):
        if not os.path.exists("dev/update-version.sh"):
            raise Exception("Caution, you must call this script from the project root directory !")

############################################################
def main():
    pass

############################################################
if __name__ == "__main__":
    pass