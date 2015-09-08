######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.0.0                        #
#            DATE     : 02/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
include_directories(${CMAKE_SOURCE_DIR}/extern-deps/gmock-1.7.0/gtest/include)

######################################################
add_library(matt-init-fake OBJECT InitFake.cpp)
SET_TARGET_PROPERTIES(matt-init-fake PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
add_library(matt-init OBJECT InitMatt.cpp)
SET_TARGET_PROPERTIES(matt-init PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
add_library(matt-init-unit-tests OBJECT InitUnitTests.cpp)
SET_TARGET_PROPERTIES(matt-init PROPERTIES COMPILE_FLAGS -fPIC)