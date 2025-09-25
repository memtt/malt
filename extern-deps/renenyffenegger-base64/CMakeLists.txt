############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : extern-deps/renenyffenegger-base64/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
add_library(base64 STATIC base64.cpp)
add_executable(TestBase64 tests.cpp)
target_link_libraries(TestBase64 base64)
add_test(TestBase64 TestBase64)
