############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 03/2015
#    LICENSE  : CeCILL-C
#    FILE     : extern-deps/from-htopml/json/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat - 2015
############################################################

######################################################
set(JSON_SOURCES JsonState.cpp ConvertToJson.cpp)
set(JSON_HEADERS JsonState.h ConvertToJson.h JsonState_impl.h ConvertToJson_impl.h)

######################################################
add_library(htopml-json OBJECT ${JSON_SOURCES})
SET_TARGET_PROPERTIES(htopml-json PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
#install procedure
#INSTALL(FILES ${JSON_HEADERS} DESTINATION include/htopml)