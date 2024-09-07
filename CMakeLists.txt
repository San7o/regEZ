cmake_minimum_required(VERSION 3.20)
project(
    SimpleRegex
    VERSION 1.0
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 23 REQUIRED)

option(REGEZ_BUILD_TESTS "Build tests" OFF)
option(REGEZ_BUILD_DEBUG "Build with debug flags" OFF)

add_executable(regez src/main.cpp)
target_include_directories(regez PRIVATE include)
target_compile_options(regez PRIVATE -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion)

if (REGEZ_BUILD_TESTS)
    add_executable(tests tests/regez_tests.cpp)
    target_include_directories(tests PRIVATE include)
    target_include_directories(tests PRIVATE tests)
    if (REGEZ_BUILD_DEBUG)
        target_compile_definitions(tests PRIVATE REGEZ_DEBUG)
    endif()
endif()

if (REGEZ_BUILD_DEBUG)
    target_compile_definitions(regez PRIVATE REGEZ_DEBUG)
endif()
