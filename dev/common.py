############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : dev/gen-archive.sh
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2015 - 2024
############################################################

############################################################
import os
import sys
import shutil
import traceback
import tempfile
import subprocess
from contextlib import contextmanager
from termcolor import cprint

##########################################################
def print_exception(exception: Exception) -> None:
    dir = os.getcwd()
    cprint("-----------------------------------------------", 'red')
    cprint(''.join(traceback.format_exception(exception)), 'dark_grey')
    cprint("-----------------------------------------------", 'red')
    cprint(f"Error from command : {' '.join(sys.argv)}", 'red')
    cprint(f"Error from workdir : {dir}", 'red')
    cprint("-----------------------------------------------", 'red')
    cprint(str(exception), 'red')
    cprint("-----------------------------------------------", 'red')
    print("")

############################################################
# Run command after logging it and intercept error
def run_shell(cmd: str) -> None:
    cprint(f"  + {cmd}", "light_blue")
    subprocess.run(cmd, shell=True, check=True)

############################################################
@contextmanager
def jump_in_dir(path: str):
    '''
    An helper semantic to jump temporarily in a directory. As wrapped with python 'with' semantic,
    went back automatically on the previous one on error or success.
    '''
    # get old path
    oldpwd = os.getcwd()

    # chump in
    os.chdir(path)

    # handle return back afterward
    try:
        yield
    finally:
        os.chdir(oldpwd)

############################################################
@contextmanager
def jump_in_tmpdir():
    tmpdir = tempfile.mkdtemp()
    try:
        with jump_in_dir(tmpdir):
            yield tmpdir
    finally:
        shutil.rmtree(tmpdir)
