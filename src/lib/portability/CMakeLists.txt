######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.2.0                        #
#            DATE     : 04/2015                      #
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
