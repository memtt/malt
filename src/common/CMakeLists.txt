######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
add_library(matt-common OBJECT CodeTiming.cpp FormattedMessage.cpp Debug.cpp Options.cpp)
SET_TARGET_PROPERTIES(matt-common PROPERTIES COMPILE_FLAGS -fPIC)