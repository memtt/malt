############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.6
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/portability/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat - 2019
############################################################

############################################################
if (MALT_PORTABILITY_OS_UNIX)
	list(APPEND MALT_PORT_SRC OSUnix.cpp)
endif (MALT_PORTABILITY_OS_UNIX)

############################################################
if (MALT_HAVE_PYTHON)
	list(APPEND MALT_PORT_SRC PythonLazy.cpp)
	include_directories(${Python3_INCLUDE_DIRS})
endif (MALT_HAVE_PYTHON)

############################################################
list(APPEND MALT_PORT_SRC LinuxProcMapReader.cpp)
list(APPEND MALT_PORT_SRC CompilerGNU.cpp)
list(APPEND MALT_PORT_SRC ClockRDTSC.cpp)

############################################################
add_library(malt-port OBJECT ${MALT_PORT_SRC})
set_target_properties(malt-port PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)