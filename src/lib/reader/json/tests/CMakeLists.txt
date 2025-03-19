add_executable(TestJson TestParser.cpp TestJson.cpp)
target_link_libraries(TestJson ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-profile)