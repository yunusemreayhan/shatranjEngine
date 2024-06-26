
find_library(GTEST gtest)
find_library(GTEST_MAIN gtest_main)
find_library(GMOCK gmock)

file(GLOB TEST "*.cpp")
add_executable(dummy_chess_enginetest ${TEST})

target_link_libraries(dummy_chess_enginetest ${GTEST} ${GTEST_MAIN} ${GMOCK} dummy_chess_engine)
include_directories(../lib)


include(GoogleTest)
gtest_discover_tests(dummy_chess_enginetest) # discovers tests by asking the compiled test executable to enumerate its tests
