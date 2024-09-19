# Define sources and headers for the library
set(SOURCES
    interp_table1d.cpp
)

set(HEADERS
    interp_table1d.h
)

# Create a static library from the sources
add_library(interp_table1d STATIC ${SOURCES})
