######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#Code options
set(ENABLE_CODE_TIMING "no"      CACHE BOOL   "Enable some tricks to quickly check timings of hostspots of MATT internal code.")
#select portability mode
set(PORTABILITY_MUTEX  "PTHREAD" CACHE STRING "Select mutex implementation to use. Currently support : 'PTHREAD', 'DUMMY'.")
set(PORTABILITY_OS     "UNIX"    CACHE STRING "Select OS implementation to use. Currently support : 'UNIX'.")

######################################################
#select portability definitions
set(MATT_PORTABILITY_MUTEX_${PORTABILITY_MUTEX} yes)
set(MATT_PORTABILITY_OS_${PORTABILITY_OS} yes)
#enabling features
if (ENABLE_CODE_TIMING)
	set(MATT_ENABLE_CODE_TIMING yes)
endif(ENABLE_CODE_TIMING)

######################################################
#gen config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

######################################################
include_directories(.)
include_directories(../extern-deps/from-fftw)
include_directories(../extern-deps/from-htopml)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

######################################################
set(COMMON_OBJS $<TARGET_OBJECTS:htopml-json> $<TARGET_OBJECTS:matt-core> $<TARGET_OBJECTS:matt-common> $<TARGET_OBJECTS:matt-port>)

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
add_subdirectory(portability)