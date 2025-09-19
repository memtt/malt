#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : examples/python/step-6-intermediate-vars.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################
import numpy as np

############################################################
def load_1GB_of_data():
    return np.ones((2 ** 30), dtype=np.uint8)

############################################################
def process_data():
    return modify2(modify1(load_1GB_of_data()))

############################################################
def modify1(data):
    return data * 2

############################################################
def modify2(data):
    return data + 10

############################################################
process_data()
