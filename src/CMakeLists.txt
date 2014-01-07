include_directories(.)

add_subdirectory(json)
add_subdirectory(lib)

add_executable(test main.cpp)
target_link_libraries(test callstacktrace htopml-json dl)