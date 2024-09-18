# Define sources and headers for the library
set(SOURCES
    interp_table.cpp
)

set(HEADERS
    interp_table.h
)

# Create a static library from the sources
add_library(interp_table STATIC ${SOURCES})
