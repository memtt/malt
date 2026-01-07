############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 11/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/manpages/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2015 - 2022
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
	install(FILES malt.1 DESTINATION share/man/man1)
endif (ENABLE_PROFILER)

############################################################
if (ENABLE_PROFILER AND PYTHON3_INSTRUM_FOUND)
	install(FILES malt-python.1 DESTINATION share/man/man1)
endif ()

############################################################
#install
install(FILES malt-webview.1
	DESTINATION share/man/man1)
