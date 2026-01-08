############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 03/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/libinstrum/wrappers/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat - 2022
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
add_library(malt-wrappers OBJECT 
      WrapperCAlloc.cpp
      WrapperPython.cpp
      WrapperPthreads.cpp
      WrapperGpuAlloc.cpp
)
set_target_properties(malt-wrappers PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
INSTALL(FILES WrapperCAlloc.hpp
        DESTINATION include/malt)

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)
