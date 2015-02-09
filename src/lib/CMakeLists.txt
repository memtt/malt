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

######################################################
#Code options
set(ENABLE_CODE_TIMING   "no"       CACHE BOOL   "Enable some tricks to quickly check timings of hostspots of MATT internal code.")
set(ENABLE_TESTS         "yes"      CACHE BOOL   "Enable build of internal tests.")
#select portability mode
set(PORTABILITY_SPINLOCK "PTHREAD"  CACHE STRING "Select spinlock implementation to use. Currently support : 'PTHREAD', 'DUMMY'.")
set(PORTABILITY_MUTEX    "PTHREAD"  CACHE STRING "Select mutex implementation to use. Currently support : 'PTHREAD', 'DUMMY'.")
set(PORTABILITY_OS       "UNIX"     CACHE STRING "Select OS implementation to use. Currently support : 'UNIX'.")
set(PORTABILITY_COMPILER "GNU"      CACHE STRING "Selectot compiler support to use. Currently support : 'GNU' (also valid intel).")

######################################################
#select portability definitions
set(MATT_PORTABILITY_SPINLOCK_${PORTABILITY_SPINLOCK}  yes)
set(MATT_PORTABILITY_MUTEX_${PORTABILITY_MUTEX}        yes)
set(MATT_PORTABILITY_OS_${PORTABILITY_OS}              yes)
set(MATT_PORTABILITY_COMPILER_${PORTABILITY_COMPILER}  yes)
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
set(COMMON_OBJS 
                $<TARGET_OBJECTS:htopml-json>
                $<TARGET_OBJECTS:matt-core>
                $<TARGET_OBJECTS:matt-tools>
                $<TARGET_OBJECTS:matt-stacks> 
#                 $<TARGET_OBJECTS:matt-valprof>
                #$<TARGET_OBJECTS:matt-stack-trees>
                $<TARGET_OBJECTS:matt-common> 
                $<TARGET_OBJECTS:matt-port>
                $<TARGET_OBJECTS:matt-hooks> 
                $<TARGET_OBJECTS:iniparser>
                )
set(WRAPPERS_OBJS
                $<TARGET_OBJECTS:matt-malloc-wrapper> 
                $<TARGET_OBJECTS:matt-thread-wrapper>
                $<TARGET_OBJECTS:matt-mmap-wrapper>
                $<TARGET_OBJECTS:matt-exit-wrapper>
                $<TARGET_OBJECTS:matt-function-wrapper>
                )

######################################################
add_library(matt-base SHARED ${COMMON_OBJS}  $<TARGET_OBJECTS:matt-init-fake>)
target_link_libraries(matt-base ${LIB_MATT_DEPS})

######################################################
add_library(matt SHARED ${COMMON_OBJS} ${WRAPPERS_OBJS} $<TARGET_OBJECTS:matt-init-fake>)
target_link_libraries(matt ${LIB_MATT_DEPS})

######################################################
add_subdirectory(init)
add_subdirectory(core)
add_subdirectory(hooks)
add_subdirectory(tools)
add_subdirectory(common)
add_subdirectory(stacks)
# add_subdirectory(valprof)
add_subdirectory(wrapper)
# add_subdirectory(stack-tree)
add_subdirectory(portability)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)

######################################################
# INSTALL(TARGETS matt matt-base 
# 	ARCHIVE DESTINATION lib
# 	LIBRARY DESTINATION lib)
