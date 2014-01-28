######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
if (MATT_PORTABILITY_OS_UNIX)
	list(APPEND MATT_PORT_SRC OSUnix.cpp)
endif (MATT_PORTABILITY_OS_UNIX)

######################################################
add_library(matt-port OBJECT ${MATT_PORT_SRC})
SET_TARGET_PROPERTIES(matt-port PROPERTIES COMPILE_FLAGS -fPIC)
