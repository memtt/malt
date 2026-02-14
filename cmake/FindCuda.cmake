############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindCargo.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
# - Try to find CUDA, mostly cuda.h to get that API definition.
# Once done this will define
#  CUDA_FOUND - Cuda has been found
#  CUDA_INCLUDE_DIR - Path to find cuda.h

############################################################
set(CUDA_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE STRING "Help cmake to find cuda prefix (https://developer.nvidia.com/cuda/toolkit) into your system.")

############################################################
find_program(CUDA_NVCC_BINARY nvcc
	HINTS ${CUDA_PREFIX}/bin)

############################################################
# extra hint
get_filename_component(CUDA_BIN_DIR ${CUDA_NVCC_BINARY} DIRECTORY)
get_filename_component(CUDA_BIN_PREFIX_DIR ${CUDA_BIN_DIR} DIRECTORY)

############################################################
# search cuda.h
find_path(CUDA_INCLUDE_DIR cuda.h
	HINTS ${CUDA_PREFIX}/include ${CUDA_BIN_PREFIX_DIR}/include)

############################################################
# set
set(CUDA_INCLUDE_DIRS ${CUDA_INCLUDE_DIR})

############################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set NPM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Cuda "Cannot find CUDA. disabling the NVidia GPU instrumentation."
	CUDA_INCLUDE_DIR
	CUDA_INCLUDE_DIRS)

############################################################
mark_as_advanced(CUDA_INCLUDE_DIR CUDA_INCLUDE_DIRS)
