cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

# Set project name and version
project(interp_table VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include the src directory where interp_table.h is located
include_directories(src)

# Add subdirectory (where interp_table library is defined)
add_subdirectory(src)

# Add executable (main.cpp is in the root directory)
add_executable(main main.cpp)

# Link the executable with the static library
target_link_libraries(main interp_table)
