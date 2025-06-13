############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.0
#    DATE     : 02/2024
#    LICENSE  : CeCILL-C
#    FILE     : extern-deps/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2024
############################################################

############################################################
add_library(base64 STATIC base64.cpp)
add_executable(TestBase64 tests.cpp)
target_link_libraries(TestBase64 base64)
add_test(TestBase64 TestBase64)
