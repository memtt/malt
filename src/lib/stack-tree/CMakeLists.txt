######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MATT_STACK_TREES_SRC StackSTLMapList.cpp)

######################################################
add_library(matt-stack-trees OBJECT ${MATT_STACK_TREES_SRC})
SET_TARGET_PROPERTIES(matt-stack-trees PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)