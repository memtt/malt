############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014 - 2020
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
############################################################

############################################################
#set default
if (LIBUNWIND_FOUND)
	set(MALT_HAVE_LIBUNWIND "yes")
else (LIBUNWIND_FOUND)
	unset(MALT_HAVE_LIBUNWIND)
endif (LIBUNWIND_FOUND)

############################################################
# python
set (MALT_HAVE_PYTHON ${PYTHON3_INSTRUM_FOUND})

############################################################
#Code options
set(ENABLE_CODE_TIMING   "no"       CACHE BOOL   "Enable some tricks to quickly check timings of hostspots of MALT internal code.")

#select portability mode
set(PORTABILITY_SPINLOCK  "PTHREAD"  CACHE STRING "Select spinlock implementation to use. Currently support : 'PTHREAD', 'DUMMY'.")
set(PORTABILITY_MUTEX     "PTHREAD"  CACHE STRING "Select mutex implementation to use. Currently support : 'PTHREAD', 'DUMMY'.")
set(PORTABILITY_OS        "UNIX"     CACHE STRING "Select OS implementation to use. Currently support : 'UNIX'.")
set(PORTABILITY_COMPILER  "GNU"      CACHE STRING "Select compiler support to use. Currently support : 'GNU' (also valid intel).")
set(PORTABILITY_CLOCK     "RDTSC"    CACHE STRING "Clock to be used to represent time in MALT.")

############################################################
#select portability definitions
set(MALT_PORTABILITY_SPINLOCK_${PORTABILITY_SPINLOCK}  yes)
set(MALT_PORTABILITY_MUTEX_${PORTABILITY_MUTEX}        yes)
set(MALT_PORTABILITY_OS_${PORTABILITY_OS}              yes)
set(MALT_PORTABILITY_COMPILER_${PORTABILITY_COMPILER}  yes)
set(MALT_PORTABILITY_BACKTRACE_${PORTABILITY_BACKTRACE}  yes)
set(MALT_PORTABILITY_CLOCK_${PORTABILITY_CLOCK}        yes)
#enabling features
if (ENABLE_CODE_TIMING)
	set(MALT_ENABLE_CODE_TIMING yes)
endif(ENABLE_CODE_TIMING)
if (ENABLE_GCC_COVERAGE)
	malt_enable_gcc_coverage()
endif (ENABLE_GCC_COVERAGE)

############################################################
set(LIB_MALT_DEPS pthread dl)
if (LIBELF_FOUND)
	list(APPEND LIB_MALT_DEPS ${LIBELF_LIBRARIES})
endif (LIBELF_FOUND)
if (LIBUNWIND_FOUND)
	list(APPEND LIB_MALT_DEPS ${LIBUNWIND_LIBRARIES})
endif (LIBUNWIND_FOUND)
if (PYTHON3_INSTRUM_FOUND)
    list(APPEND LIB_MALT_DEPS ${Python3_LIBRARIES})
endif()

############################################################
#gen config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

############################################################
include_directories(.)
include_directories(../../extern-deps/from-fftw)
include_directories(../../extern-deps/iniparser/src)
include_directories(../../extern-deps/from-htopml)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
if (PYTHON3_INSTRUM_FOUND)
    include_directories(${Python3_INCLUDE_DIRS})
endif()

############################################################
set(COMMON_OBJS $<TARGET_OBJECTS:htopml-json>
                $<TARGET_OBJECTS:malt-core>
                $<TARGET_OBJECTS:malt-tools>
                $<TARGET_OBJECTS:malt-stacks>
                $<TARGET_OBJECTS:matt-stacks-v2>
                $<TARGET_OBJECTS:malt-valprof>
                $<TARGET_OBJECTS:malt-profiler>
                $<TARGET_OBJECTS:malt-wrappers>
                $<TARGET_OBJECTS:malt-stack-trees>
                $<TARGET_OBJECTS:malt-common>
                $<TARGET_OBJECTS:malt-port>
                $<TARGET_OBJECTS:malt-state>
                $<TARGET_OBJECTS:malt-injectors-ok-for-tests>
                $<TARGET_OBJECTS:iniparser>)

############################################################
add_library(malt-base SHARED ${COMMON_OBJS})
target_link_libraries(malt-base ${LIB_MALT_DEPS})
set_target_properties(malt-base PROPERTIES VERSION ${MALT_VERSION}
                                           SOVERSION ${MALT_SO_VERSION})

############################################################
add_library(malt SHARED ${COMMON_OBJS} $<TARGET_OBJECTS:malt-injectors>)
target_link_libraries(malt ${LIB_MALT_DEPS})
set_target_properties(malt PROPERTIES VERSION ${MALT_VERSION}
                                      SOVERSION ${MALT_SO_VERSION})

############################################################
add_library(malt-controler SHARED $<TARGET_OBJECTS:malt-fake-controler>)
set_target_properties(malt-controler PROPERTIES VERSION ${MALT_VERSION}
                                                SOVERSION ${MALT_SO_VERSION})

############################################################
add_subdirectory(core)
add_subdirectory(tools)
add_subdirectory(state)
add_subdirectory(common)
add_subdirectory(stacks)
add_subdirectory(valprof)
add_subdirectory(wrappers)
add_subdirectory(profiler)
add_subdirectory(injectors)
add_subdirectory(stack-tree)
add_subdirectory(portability)

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)

############################################################
INSTALL(TARGETS malt malt-base malt-controler
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
