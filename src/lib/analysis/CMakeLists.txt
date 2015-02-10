######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.0.0                        #
#            DATE     : 02/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
add_library(matt-analysis OBJECT ProcessLevelAnalysis.cpp ThreadLevelAnalysis.cpp)
SET_TARGET_PROPERTIES(matt-analysis PROPERTIES COMPILE_FLAGS -fPIC)

# ######################################################
# if (ENABLE_TESTS)
# 	add_subdirectory(tests)
# endif (ENABLE_TESTS)