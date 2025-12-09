cmake_minimum_required(VERSION 3.15...3.22)

project(
    "WISC Compiler"
    VERSION 0.3.1
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

# Prevent cmake from refetching already downloaded content
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

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

    src/main.c.str
    
    src/graphviz.cpp

    # Codegen
    src/codegen.cpp
    src/emitter.cpp
    src/collect_vars.cpp
    src/whilegen.cpp
    src/reg_alloc.cpp

    # Parser + scanner
    src/parser.cpp
    src/parser_generator.cpp
    src/dotgenerator.cpp
    src/while_parser.cpp
    src/scanner.cpp
    src/token.cpp

    # cfg managment
    src/cfg.cpp
    src/cfg_builder.cpp 
    src/cfg_graphviz.cpp
    src/cfg_codegen.cpp
    src/flow.cpp
    src/ast.cpp
    src/cgen.cpp
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
    src/ast.cpp
    src/cgen.cpp
    src/collect_vars.cpp
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
    src/ast.cpp
    src/cgen.cpp
    src/collect_vars.cpp
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
    src/cgen_main.cpp
    src/cgen.cpp
    src/ast.cpp
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
    src/ast.cpp
    src/cgen.cpp
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
                -o ${CMAKE_BINARY_DIR}/source.tar 
                HEAD
        COMMAND ${CMAKE_COMMAND} -E tar cf ${CMAKE_BINARY_DIR}/while_progs.tar 
                --format=pax
                ${CMAKE_BINARY_DIR}/while_progs
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR} 
                tar --concatenate --file=source.tar while_progs.tar
        COMMAND ${CMAKE_COMMAND} -E rename 
                ${CMAKE_BINARY_DIR}/source.tar
                ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}.tar
        COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/while_progs.tar
        DEPENDS while_programs
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Creating source archive with compiled WHILE programs"
    )
endif()

# Detect native architecture
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    set(NATIVE_ARCH "x86_64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|ARM64")
    set(NATIVE_ARCH "aarch64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "riscv64")
    set(NATIVE_ARCH "riscv64")
else()
    set(NATIVE_ARCH ${CMAKE_SYSTEM_PROCESSOR})
endif()

message(STATUS "Native architecture detected: ${NATIVE_ARCH}")

# Initialize global property to track WHILE programs
set_property(GLOBAL PROPERTY WHILE_PROGRAMS_LIST "")

function(add_while_program PROGRAM_NAME)
    set(WHILE_FILE "${PROJECT_SOURCE_DIR}/examples/while/${PROGRAM_NAME}.while")
    
    # Organize outputs into while_progs directory structure
    set(WHILE_PROGS_DIR "${CMAKE_BINARY_DIR}/while_progs")
    set(ASM_DIR "${WHILE_PROGS_DIR}/asm")
    set(C_DIR "${WHILE_PROGS_DIR}/c_code")
    set(RISCV_BIN_DIR "${WHILE_PROGS_DIR}/bin/riscv64")
    set(NATIVE_BIN_DIR "${WHILE_PROGS_DIR}/bin/${NATIVE_ARCH}")
    
    set(ASM_FILE "${ASM_DIR}/${PROGRAM_NAME}.s")
    set(MAIN_FILE "${C_DIR}/${PROGRAM_NAME}_main.c")
    set(MAIN_C_STR "${PROJECT_SOURCE_DIR}/src/main.c.str")
    set(C_FILE_AST "${C_DIR}/${PROGRAM_NAME}_ast.c")
    set(C_CFG_FILE "${C_DIR}/${PROGRAM_NAME}_cfg.c")
    
    set(RISCV_EXEC "${RISCV_BIN_DIR}/${PROGRAM_NAME}_risc")
    set(RISCV_C_EXEC "${RISCV_BIN_DIR}/${PROGRAM_NAME}_c_ast_risc")
    set(RISCV_C_CFG_EXEC "${RISCV_BIN_DIR}/${PROGRAM_NAME}_c_cfg_risc")
    
    # Create output directories
    file(MAKE_DIRECTORY ${ASM_DIR})
    file(MAKE_DIRECTORY ${C_DIR})
    file(MAKE_DIRECTORY ${RISCV_BIN_DIR})
    file(MAKE_DIRECTORY ${NATIVE_BIN_DIR})
    
    add_custom_command(
        OUTPUT ${MAIN_C_STR}
        DEPENDS ${PROJECT_SOURCE_DIR}/src/main.c
        COMMAND ${PROJECT_SOURCE_DIR}/scripts/build_main_str.sh < ${PROJECT_SOURCE_DIR}/src/main.c > ${MAIN_C_STR}
    )

    # Custom command to generate assembly from WHILE code
    add_custom_command(
        OUTPUT ${ASM_FILE}
        OUTPUT ${MAIN_FILE}
        COMMAND wisc ${WHILE_FILE} -o ${ASM_FILE} -m ${MAIN_FILE}
        DEPENDS ${MAIN_C_STR}
        DEPENDS ${WHILE_FILE}
        DEPENDS wisc
        COMMENT "Compiling ${PROGRAM_NAME}.while to RISC-V assembly"
    )
    
    # Custom command to build the RISC-V executable
    add_custom_command(
        OUTPUT ${RISCV_EXEC}
        COMMAND riscv64-linux-gnu-gcc ${ASM_FILE} ${MAIN_FILE} 
                -DPROGRAM_NAME=${PROGRAM_NAME} -o ${RISCV_EXEC} -static
        DEPENDS ${ASM_FILE} ${MAIN_FILE}
        COMMENT "Linking ${PROGRAM_NAME} with RISC-V toolchain"
    )
    
    # Custom command to generate C code from WHILE code
    add_custom_command(
        OUTPUT ${C_FILE_AST}
        COMMAND wisc ${WHILE_FILE} -cc -o ${C_FILE_AST}
        DEPENDS ${WHILE_FILE}
        DEPENDS wisc
	COMMENT "Generating C code from AST for ${PROGRAM_NAME}.while"
    )

    add_custom_command(
	OUTPUT ${C_CFG_FILE}
	COMMAND wisc ${WHILE_FILE} -cc --c-source cfg -o ${C_CFG_FILE}
        DEPENDS ${WHILE_FILE}
        DEPENDS wisc
	COMMENT "Generating C code from CFG for ${PROGRAM_NAME}.while"
    )

    # Create RISC-V Target from assembly
    add_custom_target(${PROGRAM_NAME}_risc  DEPENDS ${RISCV_EXEC})
    
    # Create C executable from generated C code (native architecture)
    add_executable(${PROGRAM_NAME}_c ${C_FILE_AST} ${MAIN_FILE})
    target_compile_definitions(${PROGRAM_NAME}_c PRIVATE PROGRAM_NAME=${PROGRAM_NAME})
    set_target_properties(${PROGRAM_NAME}_c PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${NATIVE_BIN_DIR}
    )

    add_executable(${PROGRAM_NAME}_c_cfg ${C_CFG_FILE} ${MAIN_FILE})
    target_compile_definitions(${PROGRAM_NAME}_c_cfg PRIVATE PROGRAM_NAME=${PROGRAM_NAME})
    set_target_properties(${PROGRAM_NAME}_c_cfg PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${NATIVE_BIN_DIR}
    )
    
    # Create RISC-V C executables (cross-compiled)
    add_custom_command(
        OUTPUT ${RISCV_C_EXEC}
        COMMAND riscv64-linux-gnu-gcc ${C_FILE_AST} ${MAIN_FILE}
                -DPROGRAM_NAME=${PROGRAM_NAME} -o ${RISCV_C_EXEC} -static
        DEPENDS ${C_FILE_AST} ${MAIN_FILE}
        COMMENT "Cross-compiling C version of ${PROGRAM_NAME} to RISC-V"
    )
    
    add_custom_command(
        OUTPUT ${RISCV_C_CFG_EXEC}
        COMMAND riscv64-linux-gnu-gcc ${C_CFG_FILE} ${MAIN_FILE}
                -DPROGRAM_NAME=${PROGRAM_NAME} -o ${RISCV_C_CFG_EXEC} -static
        DEPENDS ${C_CFG_FILE} ${MAIN_FILE}
        COMMENT "Cross-compiling CFG C version of ${PROGRAM_NAME} to RISC-V"
    )
    
    add_custom_target(${PROGRAM_NAME}_c_ast_risc DEPENDS ${RISCV_C_EXEC})
    add_custom_target(${PROGRAM_NAME}_c_cfg_risc DEPENDS ${RISCV_C_CFG_EXEC})
    
    # Create custom target that depends on ALL versions
    add_custom_target(${PROGRAM_NAME} ALL DEPENDS 
        ${RISCV_EXEC} 
        ${PROGRAM_NAME}_c 
        ${PROGRAM_NAME}_c_cfg
        ${PROGRAM_NAME}_c_ast_risc
        ${PROGRAM_NAME}_c_cfg_risc
    )
    
    # Register this program in the global list
    set_property(GLOBAL APPEND PROPERTY WHILE_PROGRAMS_LIST ${PROGRAM_NAME})
endfunction()

# Empty program edge cases
# add_while_program(empty) # Parse error at Segmentation fault
# add_while_program(comment) # Parse error at Segmentation fault
# add_while_program(skip1) # Segmentation fault
add_while_program(skip2)

# Normal programs
add_while_program(add)
add_while_program(chain_prime)
add_while_program(closest_prime)
add_while_program(collatz)
# add_while_program(comment) # Parse error / Segmentation fault
add_while_program(comparatornetwork_bestknown_for_11)
add_while_program(countTriangularNumbers)
add_while_program(factorial)
add_while_program(fibonacci)
add_while_program(isprime)
add_while_program(loop)
add_while_program(madprimes)
add_while_program(primescounter)
add_while_program(pseudomatrices)
add_while_program(quad2)
add_while_program(quadratic)
add_while_program(stress_factorial)
add_while_program(stress_gcd_mod)
add_while_program(stress_gcd_sub)
add_while_program(stress_prime_count)
add_while_program(sumfactors)
# add_while_program(torture)

# Automatically create a target that builds all WHILE programs
get_property(ALL_WHILE_PROGRAMS GLOBAL PROPERTY WHILE_PROGRAMS_LIST)
add_custom_target(while_programs
    DEPENDS ${ALL_WHILE_PROGRAMS}
    COMMENT "Building all WHILE programs"
)

# Target to package RISC-V binaries
add_custom_target(package-riscv
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/riscv_binaries
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/while_progs/bin/riscv64 ${CMAKE_BINARY_DIR}/riscv_binaries
    COMMAND cd ${CMAKE_BINARY_DIR} && tar -czf riscv_binaries.tar.gz riscv_binaries
    DEPENDS while_programs
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Packaging all RISC-V binaries (assembly, C, and CFG versions)"
)
