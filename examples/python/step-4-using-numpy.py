#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : examples/python/step-4-using-numpy.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

###########################################################
import numpy

###########################################################
N = 4000000

###########################################################
def gen_vector(factor: float):
    a = numpy.ones(N, dtype=numpy.float64)
    k = 0
    for k in range(N):
        a[k] = k * factor
    return a

###########################################################
def main():
    a = gen_vector(2.5)
    b = gen_vector(5.8)
    c = a + b
    d = numpy.sum(c)
    print(d)

###########################################################
if __name__ == "__main__":
    main()

