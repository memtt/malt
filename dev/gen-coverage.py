#!/usr/bin/env python3
############################################################
#    PROJECT  : Gray Scott Simulation
#    VERSION  : 1.2.5
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : dev/gen-coverage.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2024
############################################################

############################################################
# This script aimed at making easier to quickly generate
# coverage results

############################################################
from common import print_exception, run_shell_cmds

############################################################
def malt_gen_coverage() -> None:
    run_shell_cmds([
        "lcov -o out.info --ignore-errors mismatch -c -d .",
        "lcov -o out.info --ignore-errors unused --remove out.info '/usr/*' '*/Test*' '*/tests/*' '*/extern-deps/*'",
        "genhtml -o html out.info"
    ])   
   
############################################################
if __name__ == "__main__":
    try:
        malt_gen_coverage()
        print("")
        print("Ready to browse : ./html/index.html")
    except Exception as e:
        print_exception(e)
