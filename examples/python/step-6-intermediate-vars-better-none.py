#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 10/2025
#    LICENSE  : CeCILL-C
#    FILE     : examples/python/step-6-intermediate-vars-better-none.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################
import numpy as np

############################################################
def load_1GB_of_data():
    return np.ones((2 ** 30), dtype=np.uint8)

############################################################
def process_data():
    data = load_1GB_of_data()
    tmp1 = modify1(data)
    data = None
    tmp2 = modify2(tmp1)
    tmp1 = None
    return tmp2

############################################################
def modify1(data):
    return data * 2

############################################################
def modify2(data):
    return data + 10

############################################################
process_data()
