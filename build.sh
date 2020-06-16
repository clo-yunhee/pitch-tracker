#!/bin/sh

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE
make -j$(nproc)
