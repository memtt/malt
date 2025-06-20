############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindZmq.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
# - Try to find cppzmq (http://directory.fsf.org/wiki/Libelf)
# Once done this will define
#  ZMQ_FOUND - System has cppzmq
#  ZMQ_INCLUDE_DIRS - The cppzmq include directories
#  ZMQ_LIBRARIES - The libraries needed to use cppzmq
#  ZMQ_DEFINITIONS - Compiler switches required for using cppzmq

############################################################
set(ZMQ_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE STRING "Help cmake to find cppzmq library (https://zeromq.org/) into your system.")

############################################################
find_path(ZMQ_INCLUDE_DIR cppzmq.hpp
	HINTS ${ZMQ_PREFIX}/include)

############################################################
find_library(ZMQ_LIBRARY NAMES zmq
	HINTS ${ZMQ_PREFIX}/${LIBDIR})

############################################################
set(ZMQ_LIBRARIES ${ZMQ_LIBRARY} )
set(ZMQ_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR} )

############################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ZMQ_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Zmq  DEFAULT_MSG
	ZMQ_LIBRARY ZMQ_INCLUDE_DIR)

############################################################
mark_as_advanced(ZMQ_INCLUDE_DIR ZMQ_LIBRARY )
