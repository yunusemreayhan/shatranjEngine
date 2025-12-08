set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")

add_executable(movedump ${HEADERS} ${SOURCES})

include_directories(../../lib/shatranj_simple)
include_directories(../../lib/stockfish)
include_directories(../../lib/stockfish/custom)
target_link_libraries(movedump dummy_chess_engine)