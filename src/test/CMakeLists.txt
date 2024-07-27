
if(MSVC)
    find_library(GTEST GTest::gtest)
    find_library(GTEST_MAIN GTest::gtest_main)
    find_library(GMOCK GTest::gmock)
    find_library(GMOCK_MAIN GTest::gmock_main)
else()
    find_library(GTEST gtest)
    find_library(GTEST_MAIN gtest_main)
    find_library(GMOCK gmock)
endif()

file(GLOB TEST "*.cpp")
add_executable(dummy_chess_enginetest ${TEST})

target_link_libraries(dummy_chess_enginetest ${GTEST} ${GTEST_MAIN} ${GMOCK} dummy_chess_engine)
include_directories(../lib/shatranj_simple)
include_directories(../lib/stockfish)
include_directories(../lib/stockfish/custom)

enable_testing()

include(GoogleTest)
gtest_discover_tests(dummy_chess_enginetest)
