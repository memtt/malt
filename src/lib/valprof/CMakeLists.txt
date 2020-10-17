######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 10/2020                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MALT_VALPROF_SRC ProfiledStateValue.cpp Scatter2DValues.cpp)

######################################################
add_library(malt-valprof OBJECT ${MALT_VALPROF_SRC})
SET_TARGET_PROPERTIES(malt-valprof PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)