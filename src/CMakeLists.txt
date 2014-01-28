######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#select portability mode
set(PORTABILITY_MUTEX "PTHREAD" CACHE STRING "Select mutex implementation to use. Currently support : 'PTHREAD', 'DUMMY'.")

######################################################
#select portability definitions
set(MATT_PORTABILITY_MUTEX_${PORTABILITY_MUTEX} yes)

######################################################
#gen config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

######################################################
include_directories(.)
include_directories(../extern-deps/from-fftw)
include_directories(../extern-deps/from-htopml)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

######################################################
set(COMMON_OBJS $<TARGET_OBJECTS:htopml-json> $<TARGET_OBJECTS:matt-core> $<TARGET_OBJECTS:matt-common>)

######################################################
add_library(matt-base SHARED ${COMMON_OBJS})

######################################################
add_library(matt SHARED ${COMMON_OBJS} $<TARGET_OBJECTS:matt-libc-wrapper>)
target_link_libraries(matt dl)

######################################################
add_subdirectory(core)

add_subdirectory(tests)
add_subdirectory(common)
add_subdirectory(stackv2)
