############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/wrapper/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat - 2014 - 2022
############################################################

############################################################
include_directories(${Python3_INCLUDE_DIRS})

############################################################
add_library(malt-wrappers OBJECT 
      ThreadTracker.cpp
      WrapperCAlloc.cpp
      WrapperPythonAlloc.cpp
      WrapperPthreads.cpp
      GlobalState.cpp
      InjectPthreads.cpp
)
SET_TARGET_PROPERTIES(malt-wrappers PROPERTIES COMPILE_FLAGS -fPIC)

############################################################
add_library(malt-injectors OBJECT
      InjectCAlloc.cpp
      InjectPythonAlloc.cpp
      InjectCInit.cpp
      InjectFuncEnterExit.cpp
      InjectMaqao.cpp
)
SET_TARGET_PROPERTIES(malt-injectors PROPERTIES COMPILE_FLAGS -fPIC)

############################################################
add_library(malt-fake-controler OBJECT FakeControler.c)
SET_TARGET_PROPERTIES(malt-fake-controler PROPERTIES COMPILE_FLAGS -fPIC)

############################################################
INSTALL(FILES malt.h
              WrapperCAlloc.hpp
        DESTINATION include/malt)

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)
