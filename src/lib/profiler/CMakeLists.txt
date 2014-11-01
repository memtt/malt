######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MATT_LIB_SRC AllocStackProfiler.cpp
	             LocalAllocStackProfiler.cpp)

######################################################
add_library(matt-profiler OBJECT ${MATT_LIB_SRC})
SET_TARGET_PROPERTIES(matt-profiler PROPERTIES COMPILE_FLAGS -fPIC)
