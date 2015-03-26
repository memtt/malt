######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.1.0                        #
#            DATE     : 03/2015                      #
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
                                 TraceReaderFragmentation.cpp)
