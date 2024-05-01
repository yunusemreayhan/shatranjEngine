
file(GLOB SOURCES "*.cc")

add_library(performance_tests STATIC ${SOURCES})

enable_testing ()
add_subdirectory(test)