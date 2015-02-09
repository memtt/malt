######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.0.0                        #
#            DATE     : 02/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
add_library(matt-hooks OBJECT ThreadHooksFake.cpp MallocHooksFake.cpp MmapHooksFake.cpp ExitHooksFake.cpp EnterExitFunctionHooksFake.cpp)
SET_TARGET_PROPERTIES(matt-hooks PROPERTIES COMPILE_FLAGS -fPIC)

