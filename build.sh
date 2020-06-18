#!/bin/bash

set -e
source ./args.sh

SRCDIR=`pwd`

mkdir -p build/$BUILD
cd build/$BUILD
${CROSS}cmake $SRCDIR -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE
make -j$(nproc)
