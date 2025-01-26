#!/bin/bash

# Define the build directory
BUILD_DIR="build"

# Remove the existing build directory
echo "Cleaning build directory..."
rm -rf $BUILD_DIR

# Recreate the build directory
echo "Recreating build directory..."
mkdir -p $BUILD_DIR

# Run CMake configuration
echo "Running CMake configuration..."
cmake -S . -B $BUILD_DIR

# Build the project with verbose output
echo "Building the project with verbose output..."
cmake --build $BUILD_DIR -- VERBOSE=1

echo "Rebuild complete!"
