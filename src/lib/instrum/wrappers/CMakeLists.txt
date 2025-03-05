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
add_library(malt-wrappers OBJECT 
      WrapperCAlloc.cpp
      WrapperPython.cpp
      WrapperPthreads.cpp
)
SET_TARGET_PROPERTIES(malt-wrappers PROPERTIES COMPILE_FLAGS -fPIC)

############################################################
INSTALL(FILES WrapperCAlloc.hpp
        DESTINATION include/malt)

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)
