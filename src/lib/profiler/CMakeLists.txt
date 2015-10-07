######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.3.0                        #
#            DATE     : 07/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MALT_LIB_SRC AllocStackProfiler.cpp
                 LocalAllocStackProfiler.cpp
                 ConverterToV2Tree.cpp)

######################################################
add_library(malt-profiler OBJECT ${MALT_LIB_SRC})
SET_TARGET_PROPERTIES(malt-profiler PROPERTIES COMPILE_FLAGS -fPIC)
