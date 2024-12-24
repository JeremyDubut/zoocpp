# Add test executable
add_executable(test_suite test.cpp)

# Automatically discover and register Google Test cases with CTest
# include(GoogleTest)
# gtest_discover_tests(test_suite)

# Link Google Test libraries
target_link_libraries(test_suite
  PRIVATE
    GTest::gtest_main
    GTest::gtest
)

# Register the PositiveTests suite as a separate test
add_test(
    NAME PositiveTests
    COMMAND test_suite --gtest_filter=ElaborationTests/PositiveTests.*
)

# Register the NegativeTests suite as a separate test
add_test(
    NAME NegativeTests
    COMMAND test_suite --gtest_filter=ElaborationTests/NegativeTests.*
)