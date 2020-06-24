#!/bin/bash

DIR=`pwd`

export ANDROID_NDK_HOME=$ANDROID_NDK
export ANDROID_HOME=$ANDROID_NDK/../..

cd SDL2/build-scripts
./androidbuild.sh org.libsdl /dev/null
cd ../build/org.libsdl/app

rm -rf jni/src
ln -s $DIR/SDL2_ttf jni/
#ln -s $DIR/SDL2_ttf/external/freetype-2.9.1 app/jni/freetype
