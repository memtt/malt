############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.2
#    DATE     : 06/2023
#    LICENSE  : CeCILL-C
#    FILE     : src/trace-reader/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Mehdi Raza Jaffery (CERN) - 2016
############################################################
######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 06/2023                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#gen config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

######################################################
include_directories(../lib)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/)
include_directories(../../extern-deps/from-fftw)

######################################################
add_executable(malt-trace-reader main.cpp
                                 TraceReader.cpp
                                 TraceReaderPrint.cpp
                                 TraceReaderHisto.cpp
                                 TraceReaderStackAllocs.cpp
                                 TraceReaderFragmentation.cpp
                                 TraceReaderStats.cpp)
