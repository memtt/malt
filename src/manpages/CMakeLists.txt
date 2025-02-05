############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.5
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/manpages/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2015 - 2022
############################################################

############################################################
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

############################################################
if (ENABLE_PROFILER)
install(FILES malt.1.bz2 DESTINATION share/man/man1)
endif (ENABLE_PROFILER)

############################################################
#install
install(FILES malt-passwd.1.bz2 malt-webview.1.bz2
        DESTINATION share/man/man1)
