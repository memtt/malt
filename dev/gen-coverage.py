#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : dev/gen-coverage.sh
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2022 - 2024
############################################################

############################################################
# This script aimed at making easier to quickly generate
# coverage results

############################################################
from common import print_exception, run_shell_cmds

############################################################
def malt_gen_coverage() -> None:
    run_shell_cmds([
        "lcov -o out.info -c -d .",
        "lcov -o out.info --remove out.info '/usr/*' '*/Test*' '*/tests/*' '*/extern-deps/*'",
        "genhtml -o html out.info"
    ])   
   
############################################################
if __name__ == "__main__":
    try:
        malt_gen_coverage()
    except Exception as e:
        print_exception(e)
