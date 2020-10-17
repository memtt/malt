######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 10/2020                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#Check that files are updated
# if (${CMAKE_CURRENT_SOURCE_DIR}/malt.ronn IS_NEWER_THAN ${CMAKE_CURRENT_SOURCE_DIR}/malt.1.bz2)
# 	message(WARNING "Please run make command into ${CMAKE_CURRENT_SOURCE_DIR} to update the manpages compiled with ronn tool")
# endif (${CMAKE_CURRENT_SOURCE_DIR}/malt.ronn IS_NEWER_THAN ${CMAKE_CURRENT_SOURCE_DIR}/malt.1.bz2)
# 
# #Check that files are updated
# if (${CMAKE_CURRENT_SOURCE_DIR}/malt-passwd.ronn IS_NEWER_THAN ${CMAKE_CURRENT_SOURCE_DIR}/malt-passwd.1.bz2)
# 	message(WARNING "Please run make command into ${CMAKE_CURRENT_SOURCE_DIR} to update the manpages compiled with ronn tool")
# endif (${CMAKE_CURRENT_SOURCE_DIR}/malt-passwd.ronn IS_NEWER_THAN ${CMAKE_CURRENT_SOURCE_DIR}/malt-passwd.1.bz2)
# 
# #Check that files are updated
# if (${CMAKE_CURRENT_SOURCE_DIR}/malt-webserver.ronn IS_NEWER_THAN ${CMAKE_CURRENT_SOURCE_DIR}/malt-webserver.1.bz2)
# 	message(WARNING "Please run make command into ${CMAKE_CURRENT_SOURCE_DIR} to update the manpages compiled with ronn tool")
# endif (${CMAKE_CURRENT_SOURCE_DIR}/malt-webserver.ronn IS_NEWER_THAN ${CMAKE_CURRENT_SOURCE_DIR}/malt-webserver.1.bz2)

######################################################
#install
install(FILES malt.1.bz2 malt-passwd.1.bz2 malt-webview.1.bz2
        DESTINATION share/man/man1)