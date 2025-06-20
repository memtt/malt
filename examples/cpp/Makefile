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
	step-1-basic \
	step-2-problem-fragmentation \
	step-3-problem-mem-on-stack \
	step-4-problem-short-life-alloc \
	step-5-problem-for-auto-missing-ref-copy \
	step-6-problem-grow-by-copy \
	step-7-problem-peak \
	step-8-problem-heavy-realloc \
	step-9-problem-tls-and-gbl-vars

############################################################
all: $(TARGETS)

############################################################
%:%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

step-6-problem-grow-by-copy: step-6-problem-grow-by-copy.cpp
	$(CXX) $(CXXFLAGS) -O2 -o $@ $<

############################################################
malt:
	for tmp in $(TARGETS); do malt ./$$tmp; done

############################################################
clean:
	rm -f $(TARGETS)
	rm -f malt-*.json

.PHONY: clean malt
