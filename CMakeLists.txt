cmake_minimum_required(VERSION 3.15...3.22)

project(
    "WISC Compiler"
    VERSION 0.2
    LANGUAGES C CXX ASM
)

# Default to release if no build type specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMMON_FLAGS "-Wall -Wextra -Werror -fno-rtti")

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
    
    src/graphviz.cpp

    # Codegen
    src/codegen.cpp
    src/emitter.cpp
    src/collect_vars.cpp
    src/whilegen.cpp

    # Parser + scanner
    src/parser.cpp
    src/parser_generator.cpp
    src/dotgenerator.cpp
    src/while_parser.cpp
    src/scanner.cpp
    src/token.cpp

    # cfg managment
    src/cfg_builder.cpp 
    src/cfg_graphviz.cpp
    src/cfg_codegen.cpp
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
    
    # Parser + scanner
    src/parser.cpp
    src/parser_generator.cpp
    src/dotgenerator.cpp
    src/while_parser.cpp
    src/scanner.cpp
    src/token.cpp
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
    src/tests/scanner_test.cpp
)

add_executable(
    cgen
    src/cgen.cpp
    src/collect_vars.cpp

    # Parser + scanner
    src/parser.cpp
    src/parser_generator.cpp
    src/dotgenerator.cpp
    src/while_parser.cpp
    src/scanner.cpp
    src/token.cpp
)

#[[
add_executable(
    whilegen
    src/whilegen.cpp

    # Parser + scanner
    src/parser.cpp
    src/parser_generator.cpp
    src/dotgenerator.cpp
    src/while_parser.cpp
    src/scanner.cpp
    src/token.cpp
)
]]

target_compile_definitions(scanner_test PRIVATE
    EXAMPLES_DIR="${CMAKE_SOURCE_DIR}/examples/while"
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

# Source Archive Target
find_package(Git QUIET)

if(GIT_FOUND)
    add_custom_target(source-archive
        COMMAND ${GIT_EXECUTABLE} archive --format=tar 
                --prefix=${PROJECT_NAME}-${PROJECT_VERSION}/ 
                -o ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}.tar 
                HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Creating source archive"
    )
endif()

# Initialize global property to track WHILE programs
set_property(GLOBAL PROPERTY WHILE_PROGRAMS_LIST "")

function(add_while_program PROGRAM_NAME)
    set(WHILE_FILE "${PROJECT_SOURCE_DIR}/examples/while/${PROGRAM_NAME}.while")
    set(ASM_FILE "${CMAKE_BINARY_DIR}/${PROGRAM_NAME}.s")
    set(C_FILE "${CMAKE_BINARY_DIR}/${PROGRAM_NAME}.c")
    set(RISCV_EXEC "${CMAKE_BINARY_DIR}/${PROGRAM_NAME}_risc")
    
    # Custom command to generate assembly from WHILE code
    add_custom_command(
        OUTPUT ${ASM_FILE}
        COMMAND wisc ${WHILE_FILE} -o ${ASM_FILE}
        DEPENDS ${WHILE_FILE}
        DEPENDS wisc
        COMMENT "Compiling ${PROGRAM_NAME}.while to RISC-V assembly"
    )
    
    # Custom command to build the RISC-V executable
    add_custom_command(
        OUTPUT ${RISCV_EXEC}
        COMMAND riscv64-linux-gnu-gcc ${ASM_FILE} ${PROJECT_SOURCE_DIR}/src/main.c 
                -DPROGRAM_NAME=${PROGRAM_NAME} -o ${RISCV_EXEC} -static
        DEPENDS ${ASM_FILE} ${PROJECT_SOURCE_DIR}/src/main.c
        COMMENT "Linking ${PROGRAM_NAME} with RISC-V toolchain"
    )
    
    # Custom command to generate C code from WHILE code
    add_custom_command(
        OUTPUT ${C_FILE}
        COMMAND cgen ${WHILE_FILE} > ${C_FILE}
        DEPENDS ${WHILE_FILE}
        DEPENDS cgen
        COMMENT "Generating C code for ${PROGRAM_NAME}.while"
    )
    
    # Create C executable from generated C code
    add_executable(${PROGRAM_NAME}_c ${C_FILE} src/main.c)
    target_compile_definitions(${PROGRAM_NAME}_c PRIVATE PROGRAM_NAME=${PROGRAM_NAME})
    
    # Create custom target that depends on BOTH versions
    add_custom_target(${PROGRAM_NAME} ALL DEPENDS ${RISCV_EXEC} ${PROGRAM_NAME}_c)
    
    # Register this program in the global list
    set_property(GLOBAL APPEND PROPERTY WHILE_PROGRAMS_LIST ${PROGRAM_NAME})
endfunction()

add_while_program(add)
add_while_program(chain_prime)
add_while_program(closest_prime)
add_while_program(collatz)
# add_while_program(comment) # Parse error at Segmentation fault
# add_while_program(comparatornetwork_bestknown_for_11) # Register overflow
# add_while_program(countTriangularNumbers) # Register overflow
add_while_program(factorial)
add_while_program(fibonacci)
add_while_program(isprime)
add_while_program(loop)
# add_while_program(madprimes) # Register overflow
add_while_program(primescounter)
# add_while_program(pseudomatrices) # Register overflow
# add_while_program(quad2) # Register overflow
# add_while_program(quadratic) # Register overflow
add_while_program(stress_factorial)
add_while_program(stress_gcd_mod)
add_while_program(stress_gcd_sub)
add_while_program(stress_prime_count)
add_while_program(sumfactors)
# add_while_program(torture) # Parse error 2:13 RBRACK

# Automatically create a target that builds all WHILE programs
get_property(ALL_WHILE_PROGRAMS GLOBAL PROPERTY WHILE_PROGRAMS_LIST)
add_custom_target(while_programs
    DEPENDS ${ALL_WHILE_PROGRAMS}
    COMMENT "Building all WHILE programs"
)
