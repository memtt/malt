######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.3.0-dev                    #
#            DATE     : 03/2020                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
include_directories(../lib)
include_directories(../../extern-deps/from-fftw)

######################################################
add_executable(malt-trace-reader main.cpp
                                 TraceReader.cpp
                                 TraceReaderPrint.cpp
                                 TraceReaderHisto.cpp
                                 TraceReaderStackAllocs.cpp
                                 TraceReaderFragmentation.cpp
                                 TraceReaderStats.cpp)
