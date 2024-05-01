
find_library(GTEST gtest)
find_library(GTEST_MAIN gtest_main)
find_library(GMOCK gmock)

file(GLOB TEST "*.cc")
add_executable(performance_tests_001 ${TEST})

target_link_libraries(performance_tests_001 ${GTEST} ${GTEST_MAIN} ${GMOCK} performance_tests)

include(Dart)
add_test (NAME performance_tests_001 COMMAND performance_tests_001)