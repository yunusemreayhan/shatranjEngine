
file(GLOB SOURCES "*.cc")

add_library(gsamples STATIC ${SOURCES})

enable_testing()
add_subdirectory(test)