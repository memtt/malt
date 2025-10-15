############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 09/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/reader/libreader/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
include_directories(.)
include_directories(..)
include_directories(../..)
include_directories(../../libinstrum)
include_directories(${NLOHMANN_JSON_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/extern-deps/from-fftw)
include_directories(${CMAKE_BINARY_DIR}/src/libinstrum)

############################################################
if (GRAPHVIZ_FOUND)
	add_definitions(-DHAVE_GRAPHVIZ)
endif()

############################################################
add_library(malt-reader SHARED
    # JSON
    json/File.cpp
    json/Parser.cpp
    json/Node.cpp
    json/String.cpp
    json/Json.cpp
    json/JsonIterator.cpp
    # LOADER
    loader/ProgressBar.cpp
    loader/FileReader.cpp
    loader/Prepare.cpp
    #FORMAT
    format/Config.cpp
    format/Run.cpp
    format/Globals.cpp
    format/Leaks.cpp
    format/Types.cpp
    format/Scatter.cpp
    format/Sites.cpp
    format/Stacks.cpp
    format/MemStats.cpp
    format/Timeline.cpp
    format/Threads.cpp
    format/Domains.cpp
    format/MaltProfile.cpp
    # EXTRACTOR
    extractors/ExtractorHelpers.cpp
    extractors/Extractor.cpp
    # CALL GRAPH
    callgraph/FuncMetrics.cpp
    callgraph/SimpleIdCache.cpp
    callgraph/CallTreeAdapter.cpp
    callgraph/CppDeclParser.cpp
    callgraph/DotCode.cpp
    callgraph/GraphGenerator.cpp
    # API
    api/Profile.cpp
    api/WebProfile.cpp
    # PUBLIC API
	public-api/malt-reader.cpp
    # Traces
    trace/TraceReader.cpp
)
target_link_libraries(malt-reader PUBLIC ${NLOHMANN_JSON_LIBRARIES})
target_link_libraries(malt-reader PRIVATE OpenMP::OpenMP_CXX)
set_target_properties(malt-reader PROPERTIES VERSION ${MALT_VERSION}
                                              SOVERSION ${MALT_SO_VERSION})
set_target_properties(malt-reader PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
add_executable(malt-reader-cmd main.cpp)
target_link_libraries(malt-reader-cmd malt-reader)

############################################################
if (ZMQ_FOUND)
	include_directories(${ZMQ_INCLUDE_DIR})
	add_executable(malt-zmq-reader main_zmq.cpp)
	target_link_libraries(malt-zmq-reader malt-reader ${ZMQ_LIBRARY})
endif()

############################################################
add_executable(malt-stream-reader main_stream.cpp)
target_link_libraries(malt-stream-reader malt-reader)

############################################################
if (ENABLE_TESTS)
    add_subdirectory(format/tests)
    add_subdirectory(extractors/tests)
    add_subdirectory(callgraph/tests)
    add_subdirectory(json/tests)
    add_subdirectory(tests)
endif()

############################################################
install(TARGETS malt-reader
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES public-api/malt-reader.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/malt)
