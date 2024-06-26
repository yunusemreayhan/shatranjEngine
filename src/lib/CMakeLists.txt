
file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")

add_library(dummy_chess_engine STATIC ${HEADERS} ${SOURCES})
