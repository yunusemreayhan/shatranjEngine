
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")

# dumps all source files
message (STATUS ${SOURCES})
message(STATUS ${HEADERS})

add_subdirectory(lib)

add_executable(dummy_chess_engine_bin ${HEADERS} ${SOURCES})
add_executable(shatranj_uci main_uci.cpp)
add_executable(shatranj_simple_uci main_simple_stockfish_uci.cpp)

include_directories(lib/shatranj_simple)
target_link_libraries(dummy_chess_engine_bin dummy_chess_engine)
target_link_libraries(shatranj_uci dummy_chess_engine)
target_link_libraries(shatranj_simple_uci dummy_chess_engine)

if(BUILD_TESTING)
    enable_testing ()
    add_subdirectory(test)
endif()
add_subdirectory(bin/fencalc)
add_subdirectory(bin/movedump)
