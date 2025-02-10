############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindPython.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2025
############################################################

############################################################
# - Try to find python command
# Once done this will define
#  PYTHON3_FOUND - System has python
#  PYTHON3_BINARY - Path to the python binary file
#  PYTHON3_CONFIG
#  PYTHON3_CFLAGS
#  PYTHON3_LDFLAGS
#  PYTHON3_INCLUDES
#  PYTHON3_LIBRARIES

############################################################
include(CheckCSourceRuns)

############################################################
# try instrumentation
message(STATUS "${PYTHON3_CFLAGS};${PYTHON3_LDFLAGS}")
set(CMAKE_REQUIRED_INCLUDES ${Python3_INCLUDE_DIRS})
set(CMAKE_REQUIRED_LIBRARIES ${Python3_LIBRARIES})
check_c_source_runs("
	#include <stdio.h>
	#include <stdlib.h>
	#include <Python.h>
	int Py_RunMain(){exit(0);}
	int main(int argc, char** argv){Py_BytesMain(argc, argv); return 1;}
" tmp_python3_instrum)
if (tmp_python3_instrum_EXITCODE EQUAL 0)
	set(PYTHON3_INSTRUM_FOUND ON)
else()
	set(PYTHON3_INSTRUM_FOUND OFF)
endif()

############################################################
# set final
set(PYTHON3_INSTRUM_FOUND ${PYTHON3_INSTRUM_FOUND} CACHE BOOL "If installed python support instrumentation used by MALT")

############################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set PYTHON3_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Python3Instrum "Cannot find Python3." PYTHON3_INSTRUM_FOUND)

############################################################
mark_as_advanced(PYTHON3_INSTRUM_FOUND)
