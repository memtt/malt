############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 05/2025
#    LICENSE  : CeCILL-C
#    FILE     : extern-deps/jemalloc/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

######################################################
# Install nodejs dependencies if needed
# then install node_modules into share directory
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/jemalloc-5.3.0)
	message(STATUS "Downloading jemalloc ....")
	message(STATUS "If not want download, please take the officiel release archive which conatins it...")
	execute_process(
		COMMAND ./prepare.sh
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
		RESULT_VARIABLE jemalloc_download_status
	)

	############################################################
	if (NOT jemalloc_download_status EQUAL 0)
		message(FATAL_ERROR "Fail to download the MALT internal JeMalloc !")
	else()
		message(NOTICE "jemalloc-malt downloaded...")
	endif()
endif ()

############################################################
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/jemalloc-build)

############################################################
message(NOTICE "Configuring jemalloc-malt...")
execute_process(
	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/jemalloc-5.3.0/configure
		--prefix=${CMAKE_CURRENT_BINARY_DIR}/jemalloc-prefix
		--with-jemalloc-prefix=malt_je_
		--with-private-namespace=malt_je_
		--with-install-suffix=-malt
		--enable-shared
		--disable-cxx
		--without-export
		--disable-doc
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/jemalloc-build
	RESULT_VARIABLE jemalloc_configure_status
)

############################################################
if (NOT jemalloc_configure_status EQUAL 0)
	message(FATAL_ERROR "Fail to configure the MALT internal JeMalloc !")
else()
	message(NOTICE "jemalloc-malt configured...")
endif()

############################################################
add_custom_target(jemalloc-malt_pic
	ALL
	COMMAND make install
	COMMENT "Building jemalloc-malt"
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/jemalloc-build
	USES_TERMINAL
)
# Specify where the library is and where to find the headers
set_target_properties(jemalloc-malt_pic
	PROPERTIES
	IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/jemalloc-prefix/lib/libjemalloc-malt_pic.a
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/extern-deps/jemalloc/jemalloc-build/include/jemalloc
	INTERFACE_LINK_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/jemalloc-prefix/lib/
)
