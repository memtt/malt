############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 09/2025
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
CXX=nvc++
CXXFLAGS=-Wall -O0 -g -acc=gpu
TARGETS=\
	step-01-vec-add

############################################################
all: $(TARGETS)

############################################################
%:%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

############################################################
malt:
	for tmp in $(TARGETS); do malt ./$$tmp; done

############################################################
clean:
	rm -f $(TARGETS)
	rm -f malt-*.json

.PHONY: clean malt
