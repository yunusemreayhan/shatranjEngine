
find_library(GTEST gtest)
find_library(GTEST_MAIN gtest_main)
find_library(GMOCK gmock)

file(GLOB TEST "*.cc")
add_executable(templatetemptytest ${TEST})

target_link_libraries(templatetemptytest ${GTEST} ${GTEST_MAIN} ${GMOCK} empty_template)

include(Dart)
add_test (NAME templatetemptytest COMMAND templatetemptytest)