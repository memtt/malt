############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 02/2014
#    LICENSE  : CeCILL-C
#    FILE     : extern-deps/iniparser/src/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
############################################################
add_library(iniparser OBJECT dictionary.c iniparser.c)
SET_TARGET_PROPERTIES(iniparser PROPERTIES COMPILE_FLAGS -fPIC)