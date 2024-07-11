
file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")
file(GLOB STOCKFISH_SOURCES "stockfish/*.cpp")

add_library(dummy_chess_engine STATIC ${HEADERS} ${SOURCES} ${STOCKFISH_SOURCES})
