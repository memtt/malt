include_directories(.)

add_subdirectory(json)
add_subdirectory(lib)

add_executable(test main.cpp)
target_link_libraries(test callstacktrace htopml-json dl)

#TMP
set(JSON_SOURCES json/JsonState.cpp json/TypeToJson.cpp)
set(CST_SOURCES lib/FuncNameDic.cpp lib/SimpleCallStack.cpp lib/SimpleStackTracer.cpp lib/AllocWrapper.cpp lib/AllocStackProfiler.cpp lib/CallStackInfo.cpp lib/SimpleCallStackNode.cpp)

add_library(AllocStackProfiler SHARED ${JSON_SOURCES} ${CST_SOURCES})
target_link_libraries(AllocStackProfiler callstacktrace htopml-json dl)