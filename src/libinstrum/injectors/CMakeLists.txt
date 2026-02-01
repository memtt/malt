############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 03/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/libinstrum/injectors/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
add_library(malt-injectors-ok-for-tests OBJECT 
      InjectPthreads.cpp
      InjectPython.cpp
      InjectPythonInit.cpp
)
set_target_properties(malt-injectors-ok-for-tests PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
add_library(malt-injectors OBJECT
      InjectCAlloc.cpp
      InjectCInit.cpp
      InjectFuncEnterExit.cpp
      InjectMaqao.cpp
      InjectGpuAlloc.cpp
)
set_target_properties(malt-injectors PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
#if (ENABLE_TESTS)
#	add_subdirectory(tests)
#endif (ENABLE_TESTS)
