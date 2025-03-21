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
add_library(malt-injectors-ok-for-tests OBJECT 
      InjectPthreads.cpp
)
set_target_properties(malt-injectors-ok-for-tests PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
add_library(malt-injectors OBJECT
      InjectCAlloc.cpp
      InjectPython.cpp
      InjectCInit.cpp
      InjectFuncEnterExit.cpp
      InjectMaqao.cpp
)
set_target_properties(malt-injectors PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
add_library(malt-injector-python OBJECT
      InjectPython.cpp
)
set_target_properties(malt-injector-python PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
#if (ENABLE_TESTS)
#	add_subdirectory(tests)
#endif (ENABLE_TESTS)
