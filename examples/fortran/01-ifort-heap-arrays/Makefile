############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.0
#    DATE     : 04/2025
#    LICENSE  : CeCILL-C
#    FILE     : examples/fortran/01-ifort-heap-arrays/Makefile
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
all:
	ifx -O0 -g -finstrument-functions -heap-arrays -o reproducer_heap main.f90
	ifx -O0 -g -finstrument-functions -o reproducer_stack main.f90

############################################################
test:
	malt ./reproducer_heap
	malt ./reproducer_stack

############################################################
clean:
	rm -f reproducer_heap reproducer_stack
	rm -f malt-*
