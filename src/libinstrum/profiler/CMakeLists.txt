############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 2.3.0
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/profiler/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
############################################################

############################################################
set(MALT_LIB_SRC AllocStackProfiler.cpp
                 LocalAllocStackProfiler.cpp
                 ConverterToV2Tree.cpp)

############################################################
add_library(malt-profiler OBJECT ${MALT_LIB_SRC})
set_target_properties(malt-profiler PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})
