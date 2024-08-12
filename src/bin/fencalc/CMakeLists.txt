
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")

# dumps all source files
message (STATUS ${SOURCES})
message(STATUS ${HEADERS})


add_executable(fen_calculator ${HEADERS} ${SOURCES})


include_directories(../../lib/shatranj_simple)
include_directories(../../lib/stockfish)
include_directories(../../lib/stockfish/custom)
target_link_libraries(fen_calculator dummy_chess_engine)

