######################################################
add_library(common OBJECT CodeTiming.cpp)
SET_TARGET_PROPERTIES(common PROPERTIES COMPILE_FLAGS -fPIC)