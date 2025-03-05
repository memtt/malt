############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat - 2015 - 2022
############################################################

############################################################
if (ENABLE_PROFILER)
    add_subdirectory(instrum)
endif()

############################################################
if (NLOHMANN_JSON_FOUND)
	add_subdirectory(reader)
endif()
