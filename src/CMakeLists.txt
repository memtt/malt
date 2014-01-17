######################################################
#select portability mode
set(PORTABILITY_MUTEX "PTHREAD" CACHE STRING "Select mutex implementation to use. Currently support : 'PTHREAD'.")

######################################################
#select portability definitions
set(PORTABILITY_MUTEX_${PORTABILITY_MUTEX} yes)

######################################################
#gen config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

######################################################
include_directories(.)
include_directories(../extern-deps/from-fftw)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

######################################################
add_library(cstbase SHARED $<TARGET_OBJECTS:htopml-json> $<TARGET_OBJECTS:libcst> $<TARGET_OBJECTS:common>)

######################################################
add_library(AllocStackProfiler SHARED $<TARGET_OBJECTS:libcst> $<TARGET_OBJECTS:htopml-json> $<TARGET_OBJECTS:common> $<TARGET_OBJECTS:allocwrapper>)
target_link_libraries(AllocStackProfiler dl)

######################################################
add_subdirectory(lib)
add_subdirectory(json)
add_subdirectory(tests)
add_subdirectory(common)
