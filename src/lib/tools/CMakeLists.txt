######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 10/2020                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MALT_LIB_TOOLS_SRC NMCmdReader.cpp BacktraceLibunwind.cpp Backtrace.cpp ProcPagemapReader.cpp)
                 
######################################################
#Optional sources
if (LIBELF_FOUND)
	list(APPEND MALT_LIB_TOOLS_SRC ELFReader.cpp)
else (LIBELF_FOUND)
	message(STATUS "libelf not found, profiling will ignore global variables !")
	list(APPEND MALT_LIB_TOOLS_SRC ELFReader_fake.cpp)
endif (LIBELF_FOUND)

######################################################
add_library(malt-tools OBJECT ${MALT_LIB_TOOLS_SRC})
SET_TARGET_PROPERTIES(malt-tools PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)