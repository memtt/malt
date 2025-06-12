############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.6
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : examples/python/Makefile
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
all:
	gcc -g -fPIC -shared step-3-calling-c-func.c -o libstep-3-calling-c-func.so

############################################################
malt:
	malt ./step-1-hello-world.py
	malt ./step-2-fill-list.py
	malt ./step-3-calling-c-func.py
	malt ./step-4-using-numpy.py
