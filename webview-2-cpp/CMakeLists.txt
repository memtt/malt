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
add_library(malt-webview-new-cpp-base STATIC lib/UserDb.cpp lib/BasicAuth.cpp)
target_link_libraries(malt-webview-new-cpp-base ${NLOHMANN_JSON_LIBRARIES} base64 ${CRYPTO_LIBRARY} ${HTTPLIB_LIBRARY})

############################################################
include_directories(${CMAKE_SOURCE_DIR}/extern-deps/renenyffenegger-base64)
add_executable(malt-webview-new-cpp main_webview.cpp)
target_link_libraries(malt-webview-new-cpp malt-profile malt-webview-new-cpp-base )

############################################################
add_executable(malt-passwd-new-cpp main_passwd.cpp lib/UserDb.cpp)
target_link_libraries(malt-passwd-new-cpp ${NLOHMANN_JSON_LIBRARIES} base64 ${CRYPTO_LIBRARY})


############################################################
add_definitions(-DTEST_DATA_DIR="${CMAKE_CURRENT_BINARY_DIR}")

############################################################
add_executable(TestWebviewNewCpp lib/tests/TestUserDb.cpp)
target_link_libraries(TestWebviewNewCpp ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-webview-new-cpp-base)
add_test(TestWebviewNewCpp TestWebviewNewCpp)
