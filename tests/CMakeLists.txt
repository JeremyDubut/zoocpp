# Add test executable
add_executable(test_suite test.cpp)

# Link Google Test libraries
target_link_libraries(test_suite
  PRIVATE
    GTest::gtest_main
    GTest::gtest
)

# Add tests to CTest
add_test(NAME ProgramTests COMMAND test_suite)