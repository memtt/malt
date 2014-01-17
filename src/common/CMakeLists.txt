######################################################
add_library(att-common OBJECT CodeTiming.cpp)
SET_TARGET_PROPERTIES(att-common PROPERTIES COMPILE_FLAGS -fPIC)