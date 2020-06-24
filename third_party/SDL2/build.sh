#!/bin/bash

DIR=`pwd`

cd SDL2/build/org.libsdl/app
$ANDROID_NDK/ndk-build -j$(nproc)

