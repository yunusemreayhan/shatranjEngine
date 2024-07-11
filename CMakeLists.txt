set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_minimum_required(VERSION 3.8)
project(genericcpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror -Wunused -Wfatal-errors")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fsanitize=address,undefined -fno-omit-frame-pointer")

set(CMAKE_CXX_COMPILER_LAUNCHER ${CMAKE_COMMAND} -E env LSAN_OPTIONS=verbosity=1:log_threads=1 ${CMAKE_CXX_COMPILER_LAUNCHER})

include(CTest)
enable_testing()
add_subdirectory(src)
