######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#Find
find_package(LibElf QUIET)
find_package(Libunwind QUIET)

######################################################
#set default
if (LIBUNWIND_FOUND)
	set(MATT_DEFAULT_BACKTRACE_MODE "LIBUNWIND")
else (LIBUNWIND_FOUND)
	set(MATT_DEFAULT_BACKTRACE_MODE "GLIBC")
endif (LIBUNWIND_FOUND)

######################################################
#Code options
set(ENABLE_CODE_TIMING   "no"       CACHE BOOL   "Enable some tricks to quickly check timings of hostspots of MATT internal code.")
set(ENABLE_TESTS         "yes"      CACHE BOOL   "Enable build of internal tests.")

#select portability mode
set(PORTABILITY_SPINLOCK  "PTHREAD"  CACHE STRING "Select spinlock implementation to use. Currently support : 'PTHREAD', 'DUMMY'.")
set(PORTABILITY_MUTEX     "PTHREAD"  CACHE STRING "Select mutex implementation to use. Currently support : 'PTHREAD', 'DUMMY'.")
set(PORTABILITY_OS        "UNIX"     CACHE STRING "Select OS implementation to use. Currently support : 'UNIX'.")
set(PORTABILITY_COMPILER  "GNU"      CACHE STRING "Select compiler support to use. Currently support : 'GNU' (also valid intel).")
set(PORTABILITY_BACKTRACE ${MATT_DEFAULT_BACKTRACE_MODE}    CACHE STRING "Select backtrace support to use. Currently support : 'GLIBC', 'LIBUNWIND'.")

######################################################
#check
if ((NOT LIBUNWIND_FOUND) AND (${PORTABILITY_BACKTRACE} EQUAL "LIBUNWIND"))
	message(FATAL_ERROR "Usage of LIBUNWIND for backtrace portability require presence of libunwind on your system, please check with -DLIBUNWIND_PREFIX=...")
endif ((NOT LIBUNWIND_FOUND) AND (${PORTABILITY_BACKTRACE} EQUAL "LIBUNWIND"))

######################################################
#select portability definitions
set(MATT_PORTABILITY_SPINLOCK_${PORTABILITY_SPINLOCK}  yes)
set(MATT_PORTABILITY_MUTEX_${PORTABILITY_MUTEX}        yes)
set(MATT_PORTABILITY_OS_${PORTABILITY_OS}              yes)
set(MATT_PORTABILITY_COMPILER_${PORTABILITY_COMPILER}  yes)
set(MATT_PORTABILITY_BACKTRACE_${PORTABILITY_BACKTRACE}  yes)
#enabling features
if (ENABLE_CODE_TIMING)
	set(MATT_ENABLE_CODE_TIMING yes)
endif(ENABLE_CODE_TIMING)
if (ENABLE_GCC_COVERAGE)
	matt_enable_gcc_coverage()
endif (ENABLE_GCC_COVERAGE)

######################################################
set(LIB_MATT_DEPS pthread dl)
if (LIBELF_FOUND)
	list(APPEND LIB_MATT_DEPS elf)
endif (LIBELF_FOUND)
if (MATT_PORTABILITY_BACKTRACE_LIBUNWIND)
	list(APPEND LIB_MATT_DEPS unwind-x86_64)
endif (MATT_PORTABILITY_BACKTRACE_LIBUNWIND)

######################################################
#gen config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

######################################################
include_directories(.)
include_directories(../../extern-deps/from-fftw)
include_directories(../../extern-deps/iniparser/src)
include_directories(../../extern-deps/from-htopml)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

######################################################
set(COMMON_OBJS $<TARGET_OBJECTS:htopml-json>
                $<TARGET_OBJECTS:matt-core>
                $<TARGET_OBJECTS:matt-tools>
                $<TARGET_OBJECTS:matt-stacks>
                $<TARGET_OBJECTS:matt-valprof>
                $<TARGET_OBJECTS:matt-profiler>
                $<TARGET_OBJECTS:matt-thread-wrapper>
                #$<TARGET_OBJECTS:matt-stack-trees>
                $<TARGET_OBJECTS:matt-common>
                $<TARGET_OBJECTS:matt-port>
                $<TARGET_OBJECTS:iniparser>)

######################################################
add_library(matt-base SHARED ${COMMON_OBJS})
target_link_libraries(matt-base ${LIB_MATT_DEPS})

######################################################
add_library(matt SHARED ${COMMON_OBJS} $<TARGET_OBJECTS:matt-libc-wrapper>)
target_link_libraries(matt ${LIB_MATT_DEPS})

######################################################
add_subdirectory(core)
add_subdirectory(tools)
add_subdirectory(common)
add_subdirectory(stacks)
add_subdirectory(valprof)
add_subdirectory(wrapper)
add_subdirectory(profiler)
add_subdirectory(stack-tree)
add_subdirectory(portability)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)

######################################################
INSTALL(TARGETS matt matt-base 
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)
