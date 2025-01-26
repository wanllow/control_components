#!/bin/bash

# Define the build directory
BUILD_DIR="build"

# Check if the build directory exists, create it if not
if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory not found. Creating $BUILD_DIR..."
    mkdir -p $BUILD_DIR
fi

# Run CMake configuration if the cache doesn't exist
if [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
    echo "CMake cache not found. Running CMake configuration..."
    cmake -S . -B $BUILD_DIR
fi

# Perform the build
echo "Building the project..."
cmake --build $BUILD_DIR -- -j$(nproc)

echo "Build complete!"
