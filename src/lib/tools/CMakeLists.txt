######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MATT_LIB_TOOLS_SRC NMCmdReader.cpp)
                 
######################################################
#Optional sources
if (LIBELF_FOUND)
	list(APPEND MATT_LIB_TOOLS_SRC ELFReader.cpp)
else (LIBELF_FOUND)
	message(STATUS "libelf not found, profiling will ignore global variables !")
	list(APPEND MATT_LIB_TOOLS_SRC ELFReader_fake.cpp)
endif (LIBELF_FOUND)

######################################################
add_library(matt-tools OBJECT ${MATT_LIB_TOOLS_SRC})
SET_TARGET_PROPERTIES(matt-tools PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)