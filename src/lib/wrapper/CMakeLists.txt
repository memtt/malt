######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.0.0                        #
#            DATE     : 02/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
add_library(matt-exit-wrapper OBJECT ExitWrapper.cpp)
SET_TARGET_PROPERTIES(matt-exit-wrapper PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
add_library(matt-mmap-wrapper OBJECT MmapWrapper.cpp)
SET_TARGET_PROPERTIES(matt-mmap-wrapper PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
add_library(matt-malloc-wrapper OBJECT MallocWrapper.cpp)
SET_TARGET_PROPERTIES(matt-malloc-wrapper PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
add_library(matt-thread-wrapper OBJECT PThreadWrapper.cpp)
SET_TARGET_PROPERTIES(matt-thread-wrapper PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
add_library(matt-function-wrapper OBJECT EnterExitFunction.cpp)
SET_TARGET_PROPERTIES(matt-function-wrapper PROPERTIES COMPILE_FLAGS -fPIC)
