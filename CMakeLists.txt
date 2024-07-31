set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_minimum_required(VERSION 3.8)
project(genericcpp)

if(MSVC)
    cmake_policy(SET CMP0091 NEW)

    include("build/generators/conan_toolchain.cmake")

    message("finding package GTEST")
    find_package(GTest REQUIRED)
    message("found package GTEST")

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/Wall)
        add_compile_options(/O3)
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Wall)
        add_compile_options(/O0)
    endif()

    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)

    if(_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror -Wunused -Wfatal-errors -mavx2 -DUSE_POPCNT -funroll-loops")
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fsanitize=address,undefined -fno-omit-frame-pointer")
endif()

set(CMAKE_CXX_COMPILER_LAUNCHER ${CMAKE_COMMAND} -E env LSAN_OPTIONS=verbosity=1:log_threads=1 ${CMAKE_CXX_COMPILER_LAUNCHER})

include(CTest)
enable_testing()
add_subdirectory(src)
