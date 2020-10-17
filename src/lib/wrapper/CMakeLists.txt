######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 10/2020                      #
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

######################################################
add_library(malt-fake-controler OBJECT FakeControler.c)
SET_TARGET_PROPERTIES(malt-fake-controler PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
INSTALL(FILES malt.h
	DESTINATION include/malt)
