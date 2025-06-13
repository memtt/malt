############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.0
#    DATE     : 04/2025
#    LICENSE  : CeCILL-C
#    FILE     : examples/python/main4.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
import numpy
def main():
	N = 100000
	a = [None] * N
	for i in range(N):
		a[i] = 25.5
	b = []
	for i in range(N):
		b.append(25.5)
	c = numpy.array([0]*N)

############################################################
main()
