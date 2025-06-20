############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 01/2016
#    LICENSE  : CeCILL-C
#    FILE     : src/libinstrum/valprof/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat - 2016
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