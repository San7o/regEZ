cmake_minimum_required(VERSION 3.20)
project(
    SimpleRegex
    VERSION 1.0
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 23 REQUIRED)

add_executable(sr src/main.cpp)
target_include_directories(sr PRIVATE include)
target_compile_options(sr PRIVATE -Wall -Wextra -Wpedantic -Werror)
