############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
CXX=g++
CXXFLAGS=-Wall -O0 -g -fopenmp -finstrument-functions
TARGETS=step-1-basic step-2-problem-fragmentation step-3-problem-mem-on-stack step-4-problem-short-life-alloc step-5-problem-for-auto-missing-ref-copy step-6-problem-grow-by-copy step-7-problem-peak

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

.PHONY: clean malt
