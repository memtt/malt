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
add_library(cstbase SHARED $<TARGET_OBJECTS:htopml-json> $<TARGET_OBJECTS:att-core> $<TARGET_OBJECTS:att-common>)

######################################################
add_library(AllocStackProfiler SHARED $<TARGET_OBJECTS:att-core> $<TARGET_OBJECTS:htopml-json> $<TARGET_OBJECTS:att-common> $<TARGET_OBJECTS:att-libc-wrapper>)
target_link_libraries(AllocStackProfiler dl)

######################################################
add_subdirectory(core)
add_subdirectory(json)
add_subdirectory(tests)
add_subdirectory(common)
