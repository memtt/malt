######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MATT_VALPROF_SRC ProfiledStateValue.cpp)

######################################################
add_library(matt-valprof OBJECT ${MATT_VALPROF_SRC})
SET_TARGET_PROPERTIES(matt-valprof PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)