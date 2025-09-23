############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
include_directories(../libreader)
include_directories(${HTTPLIB_INCLUDE_DIR})
include_directories(${CRYPTO_INCLUDE_DIR})
include_directories(${NLOHMANN_JSON_INCLUDE_DIR})

############################################################
add_definitions(-DMALT_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

############################################################
add_library(malt-webview-cpp-base STATIC lib/UserDb.cpp lib/BasicAuth.cpp lib/ArgChecker.cpp)
target_link_libraries(malt-webview-cpp-base ${NLOHMANN_JSON_LIBRARIES} base64 ${CRYPTO_LIBRARY} ${HTTPLIB_LIBRARY})

############################################################
include_directories(${CMAKE_SOURCE_DIR}/extern-deps/renenyffenegger-base64)
add_executable(malt-webview-cpp main_webview.cpp)
target_link_libraries(malt-webview-cpp malt-profile malt-webview-cpp-base )

############################################################
add_executable(malt-passwd-cpp main_passwd.cpp lib/UserDb.cpp)
target_link_libraries(malt-passwd-cpp ${NLOHMANN_JSON_LIBRARIES} base64 ${CRYPTO_LIBRARY})

############################################################
add_definitions(-DTEST_DATA_DIR="${CMAKE_CURRENT_BINARY_DIR}")

############################################################
if (ENABLE_TESTS)
	add_executable(TestWebviewNewCpp lib/tests/TestUserDb.cpp)
	target_link_libraries(TestWebviewNewCpp ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-webview-cpp-base)
	add_test(TestWebviewNewCpp TestWebviewNewCpp)
endif()

############################################################
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/malt-webview-cpp DESTINATION ${CMAKE_INSTALL_BINDIR} RENAME malt-webview)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/malt-passwd-cpp DESTINATION ${CMAKE_INSTALL_BINDIR} RENAME malt-passwd)
