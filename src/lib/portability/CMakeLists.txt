######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 06/2023                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
if (MALT_PORTABILITY_OS_UNIX)
	list(APPEND MALT_PORT_SRC OSUnix.cpp)
endif (MALT_PORTABILITY_OS_UNIX)

######################################################
list(APPEND MALT_PORT_SRC LinuxProcMapReader.cpp)
list(APPEND MALT_PORT_SRC CompilerGNU.cpp)

######################################################
add_library(malt-port OBJECT ${MALT_PORT_SRC})
SET_TARGET_PROPERTIES(malt-port PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)