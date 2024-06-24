set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_minimum_required(VERSION 3.8)
project(genericcpp)

enable_testing()
add_subdirectory(gtestsamples)
add_subdirectory(empty_template)
add_subdirectory(performance_tests)
add_subdirectory(src)