cmake_minimum_required(VERSION 3.20)
project(
    regez 
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23 REQUIRED)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(REGEZ_SOURCES)
set(REGEZ_HEADERS include)
set(REGEZ_COMPILE_OPTIONS -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow)
set(REGEZ_TEST_SOURCES
        tests/regez_test.cpp
        fuzz/regez_fuzz.cpp
        benchmarks/regez_benchmark.cpp
)
set(REGEZ_TEST_HEADERS)
set(REGEZ_LINK_LIBRARIES)

option(REGEZ_USE_CLANG "Use clang" OFF)
option(REGEZ_BUILD_TESTS "Build tests" ON)
option(REGEZ_BUILD_SHARED "Build shared library" OFF)
option(REGEZ_BUILD_STATIC "Build static library" OFF)
option(REGEZ_BUILD_OPTIMIZED "Build optimized" OFF)
option(REGEZ_BUILD_OPTIMIZED_AGGRESSIVE "Build with maximum optimization" OFF)

set(ENV{CPM_SOURCE_CACHE} ${CMAKE_CURRENT_SOURCE_DIR}/deps)
include(cmake/CPM.cmake) # <3
CPMUsePackageLock(package-lock.cmake)
CPMAddPackage(
  NAME Ccache.cmake
  GITHUB_REPOSITORY TheLartians/Ccache.cmake
  VERSION 1.2
)
CPMAddPackage(
    NAME valfuzz
    GITHUB_REPOSITORY San7o/valFuzz
    GIT_TAG v1.1.0-alpha
    DOWNLOAD_ONLY True
)
if (valfuzz_ADDED)
    list(APPEND REGEZ_TEST_SOURCES ${valfuzz_SOURCE_DIR}/src/valfuzz.cpp)
    list(APPEND REGEZ_TEST_HEADERS ${valfuzz_SOURCE_DIR}/include)
endif()
if (REGEZ_BUILD_OPTIMIZED)
    list(APPEND REGEZ_COMPILE_OPTIONS -O3)
endif()
if (REGEZ_BUILD_OPTIMIZED_AGGRESSIVE)
    list(APPEND REGEZ_COMPILE_OPTIONS -march=native -Ofast)
endif()

if(REGEZ_USE_CLANG)
    set(CMAKE_CXX_COMPILER clang++)
    list(APPEND REGEZ_COMPILE_OPTIONS -std=c++23 -fexperimental-library)
    list(APPEND REGEZ_LINK_LIBRARIES -fexperimental-library)
endif()

if(REGEZ_BUILD_SHARED)
    add_library(regez_shared SHARED ${REGEZ_SOURCES})
    target_include_directories(regez_shared PRIVATE ${REGEZ_HEADERS})
    target_compile_options(regez_shared PRIVATE ${REGEZ_COMPILE_OPTIONS})
endif()

if(REGEZ_BUILD_STATIC)
    add_library(regez_static STATIC ${REGEZ_SOURCES})
    target_include_directories(regez_static PRIVATE ${REGEZ_HEADERS})
    target_compile_options(regez_static PRIVATE ${REGEZ_COMPILE_OPTIONS})
endif()

if(REGEZ_BUILD_TESTS AND valfuzz_ADDED)
    add_executable(tests ${REGEZ_SOURCES} ${REGEZ_TEST_SOURCES})
    target_include_directories(tests PRIVATE ${REGEZ_HEADERS} ${REGEZ_TEST_HEADERS})
    target_compile_options(tests PRIVATE ${REGEZ_COMPILE_OPTIONS} -DREGEZ_DEBUG)
endif()
