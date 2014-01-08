######################################################
include_directories(.)
include_directories(../extern-deps/from-fftw)

######################################################
add_subdirectory(json)
add_subdirectory(lib)

######################################################
add_library(cstbase $<TARGET_OBJECTS:htopml-json> $<TARGET_OBJECTS:libcst>)

######################################################
add_executable(test main.cpp)
target_link_libraries(test cstbase dl)

######################################################
add_library(AllocStackProfiler SHARED $<TARGET_OBJECTS:libcst> $<TARGET_OBJECTS:htopml-json>  $<TARGET_OBJECTS:allocwrapper>)
target_link_libraries(AllocStackProfiler dl)

######################################################
add_subdirectory(tests)