#!/bin/bash
# Usage: ./build.sh [debug|release]

BUILD_TYPE=${1:-debug}
BUILD_DIR="build/${BUILD_TYPE}"

echo "Building in ${BUILD_TYPE} mode..."
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE^} -B ${BUILD_DIR}
cmake --build ${BUILD_DIR}
