############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 02/2014
#    LICENSE  : CeCILL-C
#    FILE     : extern-deps/from-htopml/json/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
############################################################

######################################################
set(JSON_SOURCES JsonState.cpp ConvertToJson.cpp)
set(JSON_HEADERS JsonState.h ConvertToJson.h JsonState_impl.h ConvertToJson_impl.h)

######################################################
add_library(htopml-json OBJECT ${JSON_SOURCES})
set_target_properties(htopml-json PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

######################################################
#install procedure
#INSTALL(FILES ${JSON_HEADERS} DESTINATION include/htopml)