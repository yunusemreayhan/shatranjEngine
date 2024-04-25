
file(GLOB SOURCES "*.cc")

add_library(empty_template STATIC ${SOURCES})

enable_testing ()
add_subdirectory(test)