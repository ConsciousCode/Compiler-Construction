cmake_minimum_required(VERSION 3.15...3.22)

project(
	"WISC Compiler"
	VERSION 0.1
	LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(COMMON_FLAGS "-Wall -Wextra -Werror -fno-exceptions -fno-rtti")

# Flags by build type
set(CMAKE_CXX_FLAGS_DEBUG   "${COMMON_FLAGS} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_FLAGS} -O3 -DNDEBUG")

add_executable(wisc src/main.cpp)

add_executable(test_scanner
	src/scanner.cpp
	src/test_scanner.cpp
)

target_include_directories(test_scanner PRIVATE src)
