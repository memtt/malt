######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
include_directories(../lib)
include_directories(../../extern-deps/from-fftw)

######################################################
add_executable(matt-trace-reader main.cpp
                                 TraceReader.cpp
                                 TraceReaderPrint.cpp
                                 TraceReaderHisto.cpp
                                 TraceReaderStackAllocs.cpp
                                 TraceReaderFragmentation.cpp)
