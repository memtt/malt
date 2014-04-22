######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#Generate file with full paths
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/matt.sh.in ${CMAKE_CURRENT_BINARY_DIR}/matt)

######################################################
#install
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/matt 
        DESTINATION bin/)
INSTALL(FILES MattMPIRank.cpp DESTINATION share/matt)
