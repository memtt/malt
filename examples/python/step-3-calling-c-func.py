#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.0
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : examples/python/step-3-calling-c-func.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
# ctypes_test.py
import ctypes
import pathlib
import os

############################################################
def main():
    # Load the shared library into ctypes
    script_path = os.path.abspath(os.path.dirname(__file__))
    libname = script_path + "/libstep-3-calling-c-func.so"
    c_lib = ctypes.CDLL(libname)

    x, y = 6, 2.3
    # ctypes_test.py
    answer = c_lib.make_allocation_inside_c_func(x, ctypes.c_float(y))
    print(f"    In Python: int: {x} float {y:.1f} return val {answer:.1f}")

############################################################
main()
