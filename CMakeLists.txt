cmake_minimum_required(VERSION 3.20)
project(
    SimpleRegex
    VERSION 1.0
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 23 REQUIRED)

option(REGEZ_BUILD_TESTS "Build tests" OFF)

add_executable(regez src/main.cpp)
target_include_directories(regez PRIVATE include)
target_compile_options(regez PRIVATE -Wall -Wextra -Wpedantic -Werror)

if (REGEZ_BUILD_TESTS)
    set (Boost_USE_STATIC_LIBS OFF)
    find_package (Boost REQUIRED COMPONENTS unit_test_framework)
    include_directories (${Boost_INCLUDE_DIRS})
    add_executable(tests tests/regez_tests.cpp)
    target_include_directories(tests PRIVATE include)
    target_link_libraries(tests ${Boost_LIBRARIES})
endif()
