######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 06/2023                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
check_nodejs()

######################################################
# Find the QtWidgets library
find_package(Qt5Widgets QUIET)
find_package(Qt5WebEngineWidgets QUIET)
find_package(Qt5WebKitWidgets QUIET)
find_package(Qt5Network QUIET)

######################################################
if (Qt5WebEngineWidgets_FOUND OR Qt5WebKitWidgets_FOUND)
	set(Qt5Webkit_FOUND OK)
endif (Qt5WebEngineWidgets_FOUND OR Qt5WebKitWidgets_FOUND)

if (Qt5Widgets_FOUND AND Qt5Webkit_FOUND AND Qt5Network_FOUND)
	set(QT5_FOUND OK)
	message(STATUS "Find QT5 to build view")
else (Qt5Widgets_FOUND AND Qt5Webkit_FOUND AND Qt5Network_FOUND)
	message(STATUS "QT5 webkit not found, skip QT5 view")
endif (Qt5Widgets_FOUND AND Qt5Webkit_FOUND AND Qt5Network_FOUND)

######################################################
# check arm compilation
# check arm execution
include(CheckCSourceRuns)
CHECK_C_SOURCE_RUNS("int main() {long Rt, Rt2 = 0; asm volatile(\"mrrc p15, 1, %0, %1, c14\" : \"=r\"(Rt), \"=r\"(Rt2)); return 0;}" HAVE_ARMV7A_CNTVCT)
CHECK_C_SOURCE_RUNS("int main() {lont r; asm volatile(\"mrc p15, 0, %0, c9, c13, 0\" : \"=r\"(r) ); return 0;}" HAVE_ARMV7A_PMCCNTR)
CHECK_C_SOURCE_RUNS("int main() {long long Rt; asm volatile(\"mrs %0,  CNTVCT_EL0\" : \"=r\" (Rt)); return 0;}" HAVE_ARMV8_CNTVCT_EL0)
CHECK_C_SOURCE_RUNS("int main() {long long cc; asm volatile(\"mrs %0, PMCCNTR_EL0\" : \"=r\"(cc)); return 0;}" HAVE_ARMV8_PMCCNTR_EL0)

######################################################
if (ENABLE_PROFILER)
	add_subdirectory(lib)
endif(ENABLE_PROFILER)

######################################################
add_subdirectory(integration)
add_subdirectory(trace-reader)
add_subdirectory(manpages)
if (QT5_FOUND)
	add_subdirectory(qtview)
endif (QT5_FOUND)

######################################################
# Install the webview into share directory
INSTALL(DIRECTORY webview DESTINATION share/malt
	PATTERN webview/client_v2/node_modules EXCLUDE)
