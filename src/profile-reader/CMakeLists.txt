######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.3.0                        #
#            DATE     : 07/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
include_directories(../lib)
# include_directories(../../extern-deps/from-fftw)
# include_directories(../../extern-deps/iniparser/src)
# include_directories(../../extern-deps/from-htopml)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../lib)

######################################################
malt_enable_cxx_11()

######################################################
add_executable(malt-profile-reader main.cpp 
                                   JsonNode.cpp
                                   JsonReader.cpp
                                   JsonNodeLoader.cpp
                                   $<TARGET_OBJECTS:matt-common>
                                   $<TARGET_OBJECTS:matt-allocators>
                                   $<TARGET_OBJECTS:matt-port>
                                   $<TARGET_OBJECTS:htopml-json> 
                                   $<TARGET_OBJECTS:iniparser>)
target_link_libraries(malt-profile-reader pthread)
target_link_libraries(malt-profile-reader dl)
