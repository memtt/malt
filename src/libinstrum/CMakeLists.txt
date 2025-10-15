############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/libinstrum/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014 - 2020
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
#select portability definitions
set(MALT_PORTABILITY_SPINLOCK_${PORTABILITY_SPINLOCK}  yes)
set(MALT_PORTABILITY_MUTEX_${PORTABILITY_MUTEX}        yes)
set(MALT_PORTABILITY_OS_${PORTABILITY_OS}              yes)
set(MALT_PORTABILITY_COMPILER_${PORTABILITY_COMPILER}  yes)
set(MALT_PORTABILITY_BACKTRACE_${PORTABILITY_BACKTRACE}  yes)
set(MALT_PORTABILITY_CLOCK_${PORTABILITY_CLOCK}        yes)
set(MALT_PORTABILITY_PYTHON_${PORTABILITY_PYTHON}      yes)
#enabling features
if (ENABLE_CODE_TIMING)
	set(MALT_ENABLE_CODE_TIMING yes)
endif(ENABLE_CODE_TIMING)
if (ENABLE_CODE_LEAK)
	set(MALT_ENABLE_CODE_LEAK yes)
endif()
if (ENABLE_GCC_COVERAGE)
	malt_enable_gcc_coverage()
endif (ENABLE_GCC_COVERAGE)
if (MALTJEMALLOC_FOUND)
	set(MALT_ENABLE_INTERNAL_JEMALLOC yes)
endif()
if (ENABLE_CACHING)
	set(MALT_ENABLE_CACHING yes)
endif()

############################################################
set(LIB_MALT_DEPS pthread dl)
if (LIBELF_FOUND)
	list(APPEND LIB_MALT_DEPS ${LIBELF_LIBRARIES})
endif (LIBELF_FOUND)
if (LIBUNWIND_FOUND)
	list(APPEND LIB_MALT_DEPS ${LIBUNWIND_LIBRARIES})
endif (LIBUNWIND_FOUND)
if (MALTJEMALLOC_FOUND)
	#list(APPEND LIB_MALT_DEPS malt_je_malloc::jemalloc)
	#list(APPEND LIB_MALT_DEPS ${CMAKE_BINARY_DIR}/extern-deps/jemalloc/jemalloc-prefix/lib/libjemalloc-malt_pic.a)
endif()
if (MALT_HAVE_PYTHON AND MALT_PORTABILITY_PYTHON_NATIVE)
	list(APPEND LIB_MALT_DEPS ${Python3_LIBRARIES})
endif()
if (INIPARSER_FOUND)
	list(APPEND LIB_MALT_DEPS ${INIPARSER_LIBRARIES})
endif()

############################################################
#gen config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

############################################################
include_directories(.)
include_directories(../../extern-deps/from-fftw)
include_directories(${INIPARSER_INCLUDE_DIRS})
include_directories(../../extern-deps/from-htopml)
include_directories(../../extern-deps/from-numaprof)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
if (PYTHON3_INSTRUM_FOUND)
	include_directories(${Python3_INCLUDE_DIRS})
endif()
if (MALTJEMALLOC_FOUND)
	include_directories(${MALTJEMALLOC_INCLUDE_DIRS})
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
				${INIPARSER_OBJECT})

############################################################
add_library(malt-base SHARED ${COMMON_OBJS})
target_link_libraries(malt-base ${LIB_MALT_DEPS})
if (USE_LINK_OPTIM)
	set_property(TARGET malt-base PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
set_target_properties(malt-base PROPERTIES VERSION ${MALT_VERSION}
										   SOVERSION ${MALT_SO_VERSION})
if (ENABLE_JEMALLOC)
	target_link_libraries(malt-base jemalloc-malt_pic)
endif()

############################################################
add_library(malt SHARED ${COMMON_OBJS} $<TARGET_OBJECTS:malt-injectors>)
target_link_libraries(malt ${LIB_MALT_DEPS})
if (USE_LINK_OPTIM)
	set_property(TARGET malt PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
set_target_properties(malt PROPERTIES VERSION ${MALT_VERSION}
									  SOVERSION ${MALT_SO_VERSION})
if (ENABLE_JEMALLOC)
	target_link_libraries(malt jemalloc-malt_pic)
endif()

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
INSTALL(TARGETS malt malt-controler
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
