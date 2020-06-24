#!/bin/bash

set -e

BUILD=$1
CROSS=

case $BUILD in
    win32)
        HOST=i686-w64-mingw32.shared
        CROSS=$HOST-
        ;;
    win64)
        HOST=x86_64-w64-mingw32.shared
        CROSS=$HOST-
        ;;
    android-arm)
        ANDROID_ABI=armeabi-v7a
        HOST=armv7a-linux-androideabi
        HOST2=arm-linux-androideabi
        ;;
    android-arm64)
        ANDROID_ABI=arm64-v8a
        HOST=aarch64-linux-android
        HOST2=$HOST
        ;;
    android-x86)
        ANDROID_ABI=x86
        HOST=i686-linux-android
        HOST2=$HOST
        ;;
    android-x86_64)
        ANDROID_ABI=x86_64
        HOST=x86_64-linux-android
        HOST2=$HOST
        ;;
    *)
        BUILD=linux
        ;;
esac

if [[ $BUILD == android-* ]];
then
    ANDROID_NDK=$ANDROID_NDK
    ANDROID_API=30
    ANDROID_PLATFORM=android-$ANDROID_API
    
    TOOLCHAIN=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64

    export AR=$TOOLCHAIN/bin/$HOST2-ar
    export AS=$TOOLCHAIN/bin/$HOST2-as
    export CC=$TOOLCHAIN/bin/${HOST}${ANDROID_API}-clang
    export CXX=$TOOLCHAIN/bin/${HOST}${ANDROID_API}-clang++
    export LD=$TOOLCHAIN/bin/$HOST2-ld
    export RANLIB=$TOOLCHAIN/bin/$HOST2-ranlib
    export STRIP=$TOOLCHAIN/bin/$HOST2-strip

    INSDIR=$(dirname "$(readlink -f "$0")")/third_party/usr/$BUILD

    export PKG_CONFIG_EXE="$INSDIR/bin/pkg-config"
    export PKG_CONFIG_PATH="$INSDIR/lib/pkgconfig:$INSDIR/share/pkgconfig"

    CMAKE_ARGS="-DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake -DANDROID_NDK=$ANDROID_NDK -DANDROID_PLATFORM=$ANDROID_PLATFORM -DANDROID_ABI=$ANDROID_ABI -DPKG_CONFIG_EXECUTABLE=$PKG_CONFIG_EXE"
fi

