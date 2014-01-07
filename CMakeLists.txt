cmake_minimum_required(VERSION 2.7)

project(callstacktrace)

add_subdirectory(json)
add_executable(callstacktrace main.cpp SimpleCallStack.cpp SimpleTracer.cpp FuncNameDic.cpp)
target_link_libraries(callstacktrace dl htopml-json)
