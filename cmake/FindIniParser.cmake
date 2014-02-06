######################################################
# - Try to find iniparser (http://ndevilla.free.fr/iniparser/)
# Once done this will define
#  INIPARSER_FOUND - System has iniparser
#  INIPARSER_INCLUDE_DIRS - The iniparser include directories
#  INIPARSER_LIBRARIES - The libraries needed to use iniparser
#  INIPARSER_DEFINITIONS - Compiler switches required for using iniparser

######################################################
set(INIPARSER_PREFIX "" STRING CACHE "Help cmake to find iniparser library (http://ndevilla.free.fr/iniparser) into your system.")

######################################################
find_package(PkgConfig)
pkg_check_modules(PC_INIPARSER QUIET iniparser)
set(INIPARSER_DEFINITIONS ${PC_INIPARSER_CFLAGS_OTHER})

######################################################
find_path(INIPARSER_INCLUDE_DIR iniparser.h
	HINTS ${PC_INIPARSER_INCLUDEDIR} ${PC_INIPARSER_INCLUDE_DIRS} ${INIPARSER_PREFIX}/include
	PATH_SUFFIXES libxml2 )

######################################################
find_library(INIPARSER_LIBRARY NAMES iniparser
	HINTS ${PC_INIPARSER_LIBDIR} ${PC_INIPARSER_LIBRARY_DIRS} ${INIPARSER_PREFIX}/lib)

######################################################
set(INIPARSER_LIBRARIES ${INIPARSER_LIBRARY} )
set(INIPARSER_INCLUDE_DIRS ${INIPARSER_INCLUDE_DIR} )

######################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set INIPARSER_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(iniparser  DEFAULT_MSG
	INIPARSER_LIBRARY INIPARSER_INCLUDE_DIR)

######################################################
mark_as_advanced(INIPARSER_INCLUDE_DIR INIPARSER_LIBRARY )
