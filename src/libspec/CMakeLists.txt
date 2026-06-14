############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 04/2026
#    LICENSE  : CeCILL-C
#    FILE     : src/libspec/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2026
############################################################

############################################################
include_directories(../../extern-deps/from-fftw)

############################################################
add_library(malt-spec STATIC public-c-api/malt-trace-spec.cpp)
set_target_properties(malt-spec PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

############################################################
install(
	FILES
		public-c-api/malt-trace-spec.h
		public-c-api/malt-profile-spec.h
	DESTINATION
		${CMAKE_INSTALL_INCLUDEDIR}/malt)