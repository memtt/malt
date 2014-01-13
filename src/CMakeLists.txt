######################################################
include_directories(.)
include_directories(../extern-deps/from-fftw)

######################################################
add_library(cstbase SHARED $<TARGET_OBJECTS:htopml-json> $<TARGET_OBJECTS:libcst> $<TARGET_OBJECTS:common>)

######################################################
add_library(AllocStackProfiler SHARED $<TARGET_OBJECTS:libcst> $<TARGET_OBJECTS:htopml-json> $<TARGET_OBJECTS:common> $<TARGET_OBJECTS:allocwrapper>)
target_link_libraries(AllocStackProfiler dl)

######################################################
add_executable(basictest main.cpp)
target_link_libraries(basictest AllocStackProfiler)
set_target_properties(basictest PROPERTIES COMPILE_FLAGS -finstrument-functions)

######################################################
add_subdirectory(lib)
add_subdirectory(json)
add_subdirectory(tests)
add_subdirectory(common)