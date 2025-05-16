############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/stacks/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
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