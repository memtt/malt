######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 10/2020                      #
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
