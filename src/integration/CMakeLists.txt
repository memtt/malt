######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.0.0                        #
#            DATE     : 02/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#Generate file with full paths
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/matt.sh.in ${CMAKE_CURRENT_BINARY_DIR}/matt)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/matt-webserver.sh.in ${CMAKE_CURRENT_BINARY_DIR}/matt-webserver)

######################################################
#install
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/matt ${CMAKE_CURRENT_BINARY_DIR}/matt-webserver
        DESTINATION bin/)
INSTALL(FILES MattMPIRank.cpp DESTINATION share/matt)
