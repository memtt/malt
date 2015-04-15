######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.2.0                        #
#            DATE     : 04/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MALT_LIB_SRC AllocStackProfiler.cpp
	             LocalAllocStackProfiler.cpp)

######################################################
add_library(malt-profiler OBJECT ${MALT_LIB_SRC})
SET_TARGET_PROPERTIES(malt-profiler PROPERTIES COMPILE_FLAGS -fPIC)
