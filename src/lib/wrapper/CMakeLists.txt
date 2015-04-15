######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.2.0                        #
#            DATE     : 04/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
add_library(malt-libc-wrapper OBJECT ../wrapper/AllocWrapper.cpp)
SET_TARGET_PROPERTIES(malt-libc-wrapper PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
set(MALT_THREAD_WRAPPER_SRC ThreadTracker.cpp)

######################################################
add_library(malt-thread-wrapper OBJECT ${MALT_THREAD_WRAPPER_SRC})
SET_TARGET_PROPERTIES(malt-thread-wrapper PROPERTIES COMPILE_FLAGS -fPIC)

