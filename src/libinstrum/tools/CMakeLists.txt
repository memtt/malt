############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 03/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/libinstrum/tools/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
set(MALT_LIB_TOOLS_SRC NMCmdReader.cpp BacktraceLibunwind.cpp Backtrace.cpp ProcPagemapReader.cpp Addr2Line.cpp)
                 
############################################################
#Optional sources
if (LIBELF_FOUND)
	list(APPEND MALT_LIB_TOOLS_SRC ELFReader.cpp)
else (LIBELF_FOUND)
	message(STATUS "libelf not found, profiling will ignore global variables !")
	list(APPEND MALT_LIB_TOOLS_SRC ELFReader_fake.cpp)
endif (LIBELF_FOUND)

############################################################
add_library(malt-tools OBJECT ${MALT_LIB_TOOLS_SRC})
set_target_properties(malt-tools PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)