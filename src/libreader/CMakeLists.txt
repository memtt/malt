############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014 - 2020
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
############################################################

############################################################
include_directories(.)

############################################################
add_library(malt-profile SHARED
    loader/ProgressBar.cpp
    loader/FileReader.cpp
    loader/JsonLoader.cpp
    format/Config.cpp
    format/Run.cpp
    format/Globals.cpp
    format/Leaks.cpp
    format/Types.cpp
    format/Scatter.cpp
    format/Sites.cpp
    format/Stacks.cpp
    format/MemStats.cpp
    format/Timeline.cpp
    format/Threads.cpp
    format/MaltProfile.cpp
)
target_link_libraries(malt-profile PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(malt-profile PRIVATE OpenMP::OpenMP_CXX)

############################################################
add_executable(malt-reader main.cpp)
target_link_libraries(malt-reader malt-profile)

############################################################
if (ENABLE_TESTS)
    add_subdirectory(format/tests)
endif()
