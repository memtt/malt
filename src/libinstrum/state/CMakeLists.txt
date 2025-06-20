############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 03/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/libinstrum/state/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat - 2014 - 2022
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
add_library(malt-state OBJECT 
      GlobalState.cpp
      Enable.cpp
)
set_target_properties(malt-state PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
add_library(malt-fake-controler OBJECT FakeControler.c)
set_target_properties(malt-fake-controler PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
INSTALL(FILES malt.h
        DESTINATION include/malt)

############################################################
#if (ENABLE_TESTS)
#	add_subdirectory(tests)
#endif (ENABLE_TESTS)
