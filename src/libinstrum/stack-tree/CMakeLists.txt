############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.6
#    DATE     : 02/2025
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
set_target_properties(malt-stack-trees PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)

############################################################
add_subdirectory(from-v2)