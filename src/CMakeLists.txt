
file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")

# dumps all source files
message (STATUS ${SOURCES})
message(STATUS ${HEADERS})

add_subdirectory(lib)

add_executable(dummy_chess_engine_bin ${HEADERS} ${SOURCES})


include_directories(lib/shatranj_simple)
target_link_libraries(dummy_chess_engine_bin dummy_chess_engine)

enable_testing ()
add_subdirectory(test)
