######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.0.0                        #
#            DATE     : 02/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
if (MATT_PORTABILITY_OS_UNIX)
	list(APPEND MATT_PORT_SRC OSUnix.cpp)
endif (MATT_PORTABILITY_OS_UNIX)

######################################################
list(APPEND MATT_PORT_SRC LinuxProcMapReader.cpp)
list(APPEND MATT_PORT_SRC CompilerGNU.cpp)

######################################################
add_library(matt-port OBJECT ${MATT_PORT_SRC})
SET_TARGET_PROPERTIES(matt-port PROPERTIES COMPILE_FLAGS -fPIC)
