######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.1.0                        #
#            DATE     : 03/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MALT_LIB_TOOLS_SRC NMCmdReader.cpp BacktraceLibunwind.cpp Backtrace.cpp)
                 
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