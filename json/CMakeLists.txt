######################################################
#            PROJECT  : htopml                       #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 11/2012                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(JSON_SOURCES JsonState.cpp TypeToJson.cpp)
set(JSON_HEADERS JsonState.h TypeToJson.h JsonState_impl.h TypeToJson_impl.h)

######################################################
add_library(htopml-json ${JSON_SOURCES})
SET_TARGET_PROPERTIES(htopml-json PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
#install procedure
INSTALL(FILES ${JSON_HEADERS} DESTINATION include/htopml)