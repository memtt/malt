#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 05/2025
#    LICENSE  : CeCILL-C
#    FILE     : examples/python/step-2-fill-list.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

###########################################################
import gc

###########################################################
N = 400000

###########################################################
def fill_list_by_append():
    lst = []
    for k in range(N):
        lst.append(k)
    return lst

###########################################################
def fill_list_by_pre_alloc():
    lst = [None] * N
    for k in range(N):
        lst[k] = k
    return lst

###########################################################
def main():
    # call twice
    fill_list_by_append()
    fill_list_by_append()

    # call twice
    fill_list_by_pre_alloc()
    fill_list_by_pre_alloc()

###########################################################
if __name__ == "__main__":
    main()

