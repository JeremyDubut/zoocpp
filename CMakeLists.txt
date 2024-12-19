cmake_minimum_required(VERSION 3.10)

# Project name and version
project(ElaborationZoo VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Output directories for build artifacts
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Flex and Bison setup
find_package(FLEX REQUIRED)
find_package(BISON 3.2)

set(FLEX_OUTPUT ${CMAKE_BINARY_DIR}/lex.yy_foo_.cc)
set(BISON_OUTPUT ${CMAKE_BINARY_DIR}/parser.tab.cc)
set(BISON_HEADER ${CMAKE_BINARY_DIR}/parser.tab.hh)

# Generate the lexer and parser
FLEX_TARGET(Lexer src/lexer.ll ${FLEX_OUTPUT})
BISON_TARGET(Parser src/parser.yy ${BISON_OUTPUT} COMPILE_FLAGS "-v -Wcounterexamples --defines=${BISON_HEADER}")

# Add generated files to the project
# ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

# Specify Debug build options
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src/include ${CMAKE_BINARY_DIR})

# Source files
set(SOURCES
    src/main.cpp
    src/syntax.cpp
    src/value.cpp
    src/rsyntax.cpp
    src/metavar.cpp
    ${FLEX_Lexer_OUTPUTS}
    ${BISON_Parser_OUTPUTS}
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Compiler warnings (optional)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()


# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

# Add subdirectory for tests
enable_testing()
add_subdirectory(tests)