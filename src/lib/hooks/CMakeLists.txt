######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
add_library(matt-hooks OBJECT ThreadHooksFake.cpp MallocHooksFake.cpp MmapHooksFake.cpp)
SET_TARGET_PROPERTIES(matt-hooks PROPERTIES COMPILE_FLAGS -fPIC)

