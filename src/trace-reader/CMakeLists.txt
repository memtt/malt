############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 03/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/trace-reader/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Mehdi Raza Jaffery (CERN) - 2016
#    AUTHOR   : Sébastien Valat - 2022
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
#gen config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

############################################################
include_directories(../libinstrum)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/)
include_directories(../../extern-deps/from-fftw)

############################################################
add_executable(malt-trace-reader main.cpp
                                 TraceReader.cpp
                                 TraceReaderPrint.cpp
                                 TraceReaderHisto.cpp
                                 TraceReaderStackAllocs.cpp
                                 TraceReaderFragmentation.cpp
                                 TraceReaderStats.cpp)
