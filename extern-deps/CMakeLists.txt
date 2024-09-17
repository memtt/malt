############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.2
#    DATE     : 02/2024
#    LICENSE  : CeCILL-C
#    FILE     : extern-deps/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat - 2015
#    AUTHOR   : Sébastien Valat (INRIA) - 2024
############################################################
######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 03/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
add_subdirectory(from-htopml)
add_subdirectory(iniparser)
# only required for testing
if(ENABLE_TESTS AND GTEST_USE_EMBEDED)
	add_subdirectory(googletest-1.14.0)
endif()
