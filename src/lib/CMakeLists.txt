
file(GLOB SHATRANJ_SIMPLE_HEADERS "shatranj_simple/*.h")
file(GLOB SHATRANJ_SIMPLE_SOURCES "shatranj_simple/*.cpp")
file(GLOB STOCKFISH_SOURCES "stockfish/*.cpp" "stockfish/custom/*.cpp")
file(GLOB STOCKFISH_HEADERS "stockfish/*.h" "stockfish/custom/*.h")

add_library(dummy_chess_engine STATIC ${SHATRANJ_SIMPLE_HEADERS} ${SHATRANJ_SIMPLE_SOURCES} ${STOCKFISH_SOURCES} ${STOCKFISH_HEADERS})
