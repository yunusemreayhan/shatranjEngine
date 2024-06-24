
file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")

add_library(dummy_chess_engine STATIC ${HEADERS} ${SOURCES})

add_executable(dummy_chess_engine_bin ${HEADERS} ${SOURCES})

enable_testing ()
add_subdirectory(test)