############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.5
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/wrapper/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat - 2014 - 2022
############################################################

############################################################
add_library(malt-libc-wrapper OBJECT ../wrapper/AllocWrapper.cpp)
SET_TARGET_PROPERTIES(malt-libc-wrapper PROPERTIES COMPILE_FLAGS -fPIC)

############################################################
set(MALT_THREAD_WRAPPER_SRC ThreadTracker.cpp)

############################################################
add_library(malt-thread-wrapper OBJECT ${MALT_THREAD_WRAPPER_SRC})
SET_TARGET_PROPERTIES(malt-thread-wrapper PROPERTIES COMPILE_FLAGS -fPIC)

############################################################
add_library(malt-fake-controler OBJECT FakeControler.c)
SET_TARGET_PROPERTIES(malt-fake-controler PROPERTIES COMPILE_FLAGS -fPIC)

############################################################
INSTALL(FILES malt.h
              AllocWrapperExtend.hpp
        DESTINATION include/malt)

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)
