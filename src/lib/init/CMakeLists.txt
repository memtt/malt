######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
add_library(matt-init-fake OBJECT InitFake.cpp)
SET_TARGET_PROPERTIES(matt-init-fake PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
add_library(matt-init OBJECT InitMatt.cpp)
SET_TARGET_PROPERTIES(matt-init PROPERTIES COMPILE_FLAGS -fPIC)