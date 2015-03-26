######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.1.0                        #
#            DATE     : 03/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#Generate file with full paths
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/malt.sh.in ${CMAKE_CURRENT_BINARY_DIR}/malt)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/malt-webserver.sh.in ${CMAKE_CURRENT_BINARY_DIR}/malt-webserver)

######################################################
#install
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/malt ${CMAKE_CURRENT_BINARY_DIR}/malt-webserver
        DESTINATION bin/)
INSTALL(FILES MaltMPIRank.cpp DESTINATION share/malt)
