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
set(MALT_WRAPPER_SOURCES
      WrapperCAlloc.cpp
      WrapperPython.cpp
      WrapperPthreads.cpp
      WrapperGpuAlloc.cpp
)

############################################################
if (CUDA_FOUND)
	list(APPEND MALT_WRAPPER_SOURCES WrapperGpuCudaAlloc.cpp)
endif (CUDA_FOUND)

############################################################
add_library(malt-wrappers OBJECT ${MALT_WRAPPER_SOURCES})
set_target_properties(malt-wrappers PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
INSTALL(FILES WrapperCAlloc.hpp
        DESTINATION include/malt)

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)
