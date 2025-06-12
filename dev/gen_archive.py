#!/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.6
#    DATE     : 05/2025
#    LICENSE  : CeCILL-C
#    FILE     : dev/gen_archive.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2024
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
#This script aimed at generating archive to distribute
#the project. Simply call it from root directory without
#any agruments.

############################################################
import os
import shutil
import subprocess
from termcolor import cprint
from common import print_exception, run_shell, jump_in_dir, jump_in_tmpdir

############################################################
#extract version
PACKAGE_NAME="malt"
PACKAGE_VERSION="1.2.6"

############################################################
# Generate MALT archive
def malt_dev_gen_archive(name: str = PACKAGE_NAME, version: str = PACKAGE_VERSION) -> None:
    # build default prefix
    prefix=f"{name}-{version}"

    # check if has changes in wait
    out = subprocess.check_output("git status --porcelain | grep -v '??'", shell=True)
    if out != "":
        cprint("Warning: GIT has some uncommited changes", color="yellow")
    
    # check is not release
    git_head_hash = subprocess.getoutput("git rev-parse --short HEAD")
    git_version_hash = subprocess.getoutput(f"git rev-parse --short v{version}")
    if git_head_hash != git_version_hash:
        prefix=f"{name}-{git_head_hash}"

    # log
    print(f"Generate {prefix}.tar.bz2...")

    # gen native archive
    run_shell(f"git archive --format=tar --prefix={prefix}/ HEAD | bzip2 > /tmp/{prefix}.tar.bz2")

    # extract to fetch deps & regen
    origdir = os.getcwd()
    with jump_in_tmpdir() as dir:
        run_shell(f"tar -xf /tmp/{prefix}.tar.bz2")
        with jump_in_dir(f"{prefix}/"):
            print("  --------------------------------------------")
            run_shell("./prepare.sh 2>&1 | sed -e 's/^/  /g'")
            print("  --------------------------------------------")
        run_shell(f"tar -cjf {prefix}.tar.bz2 {prefix}")
        print(f"  + mv '{prefix}.tar.bz2' '{origdir}/{prefix}.tar.bz2'")
        shutil.move(f"{prefix}.tar.bz2", f"{origdir}/{prefix}.tar.bz2")

    # log
    print("")
    cprint("Done", "green")

############################################################
if __name__ == "__main__":
    try:
        malt_dev_gen_archive()
    except Exception as e:
        print_exception(e)
