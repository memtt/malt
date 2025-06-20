############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 05/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/libinstrum/stacks/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014 - 2015
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
set(MALT_STACKS_SRC Stack.cpp
                    LangAddress.cpp
                    BacktraceStack.cpp
                    EnterExitStack.cpp
                    StackLoopRemover.cpp
                    BacktracePythonStack.cpp
                    MultiLangStackMerger.cpp
                    StackReducer.cpp)

############################################################
add_library(malt-stacks OBJECT ${MALT_STACKS_SRC})
set_target_properties(malt-stacks PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)