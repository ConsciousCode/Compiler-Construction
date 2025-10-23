cmake_minimum_required(VERSION 3.15...3.22)

project(
	"WISC Compiler"
	VERSION 0.1
	LANGUAGES CXX
)

# Default to release if no build type specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMMON_FLAGS "-Wall -Wextra -Werror -fno-exceptions -fno-rtti")

# Flags by build type
set(CMAKE_CXX_FLAGS_DEBUG   "${COMMON_FLAGS} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_FLAGS} -O3 -DNDEBUG")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Latest ARGPARSE
include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

add_executable(wisc 
    src/main.cpp

    # Codegen
    src/codegen.cpp
    src/emitter.cpp

    # AST
    src/ast_builder.cpp

    # Parser + scanner
    src/parser.cpp
    src/parser_generator.cpp
    src/dotgenerator.cpp
    src/while_parser.cpp
    src/scanner.cpp
    src/token.cpp
)
target_compile_options(wisc PRIVATE -fexceptions)

target_link_libraries(wisc argparse)

add_executable(scanner_example
	src/scanner.cpp
	src/scanner_example.cpp
	src/token.cpp
)

add_executable(
    graphviz_test
    src/graphviz.cpp
    src/graphviz_test.cpp
    src/ast_examples.cpp
)

add_executable(
    parser_example
    src/parser_example.cpp
    src/parser.cpp
    src/parser_generator.cpp
    src/dotgenerator.cpp
    src/graphviz.cpp
    src/while_parser.cpp
    src/scanner.cpp
    src/token.cpp
)
target_include_directories(parser_example PRIVATE src)

enable_testing()

add_executable(
    scanner_test
    src/scanner.cpp
    src/token.cpp
    src/scanner_test.cpp
)

target_compile_definitions(scanner_test PRIVATE
    EXAMPLES_DIR="${CMAKE_SOURCE_DIR}/examples"
)

target_link_libraries(
    scanner_test
    GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(scanner_test)

target_include_directories(scanner_example PRIVATE src)

# Clang Formatting
find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format)

if(CLANG_FORMAT_EXECUTABLE)
    # Get all project files
    file(GLOB_RECURSE ALL_SOURCE_FILES 
        ${PROJECT_SOURCE_DIR}/src/*.cpp
        ${PROJECT_SOURCE_DIR}/src/*.hpp
        ${PROJECT_SOURCE_DIR}/include/*.h
        ${PROJECT_SOURCE_DIR}/include/*.hpp
    )
    
    # Add format target
    add_custom_target(
        format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${ALL_SOURCE_FILES}
        COMMENT "Running clang-format on source files"
    )
endif()
