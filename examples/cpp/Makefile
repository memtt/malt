############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 05/2025
#    LICENSE  : CeCILL-C
#    FILE     : examples/cpp/Makefile
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
# Note : -g  => Require to recompute the call stacks, can run
#               without but with no details
#        -O0 => Required to well see the call stacks (not inlined)
#               only, otherwise O3 is also fine.
#        -finstrument-function => Required to see the stack consumption.

############################################################
CXX=g++
CXXFLAGS=-Wall -O0 -g -fopenmp -finstrument-functions
TARGETS=\
	step-01-basic \
	step-02-problem-fragmentation \
	step-03-problem-mem-on-stack \
	step-04-problem-short-life-alloc \
	step-05-problem-for-auto-missing-ref-copy \
	step-06-problem-grow-by-copy \
	step-07-problem-peak \
	step-08-problem-heavy-realloc \
	step-09-problem-tls-and-gbl-vars \
	step-10-problem-leak \
	step-11-direct-mmap-munmap

############################################################
all: $(TARGETS)

############################################################
%:%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

step-06-problem-grow-by-copy: step-06-problem-grow-by-copy.cpp
	$(CXX) $(CXXFLAGS) -O2 -o $@ $<

############################################################
malt:
	for tmp in $(TARGETS); do malt ./$$tmp; done

############################################################
clean:
	rm -f $(TARGETS)
	rm -f malt-*.json

.PHONY: clean malt
