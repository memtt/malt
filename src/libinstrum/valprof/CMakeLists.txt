############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 2.3.0
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/valprof/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
############################################################

############################################################
set(MALT_VALPROF_SRC ProfiledStateValue.cpp Scatter2DValues.cpp)

############################################################
add_library(malt-valprof OBJECT ${MALT_VALPROF_SRC})
SET_TARGET_PROPERTIES(malt-valprof PROPERTIES COMPILE_FLAGS -fPIC)

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)