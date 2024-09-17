############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.2
#    DATE     : 06/2023
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/stack-tree/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014 - 2015
#    AUTHOR   : Sébastien Valat (ECR) - 2014
############################################################

############################################################
set(MALT_STACK_TREES_SRC StackSTLHashMap.cpp)

############################################################
add_library(malt-stack-trees OBJECT ${MALT_STACK_TREES_SRC})
SET_TARGET_PROPERTIES(malt-stack-trees PROPERTIES COMPILE_FLAGS -fPIC)

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)

############################################################
add_subdirectory(from-v2)