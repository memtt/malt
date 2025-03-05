############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/wrapper/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat - 2014 - 2022
############################################################

############################################################
add_library(malt-state OBJECT 
      GlobalState.cpp
      Enable.cpp
)
SET_TARGET_PROPERTIES(malt-state PROPERTIES COMPILE_FLAGS -fPIC)

############################################################
add_library(malt-fake-controler OBJECT FakeControler.c)
SET_TARGET_PROPERTIES(malt-fake-controler PROPERTIES COMPILE_FLAGS -fPIC)

############################################################
INSTALL(FILES malt.h
        DESTINATION include/malt)

############################################################
#if (ENABLE_TESTS)
#	add_subdirectory(tests)
#endif (ENABLE_TESTS)
