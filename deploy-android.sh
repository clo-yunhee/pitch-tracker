#!/bin/bash

set -e

args_sh=$(readlink -f "./args.sh")

source $args_sh

# Make sure it's built first.
for target in android-arm android-arm64 android-x86 android-x86_64;
do
    ./build.sh $target
done

SRC_DIR=`pwd`
DEST_DIR=`pwd`/dist/android

rm -rf $DEST_DIR
mkdir -p $DEST_DIR
cd $DEST_DIR

cp -r $DEST_DIR/../res/android-project $DEST_DIR/android-project

for target in android-arm android-arm64 android-x86 android-x86_64;
do
    source $args_sh $target

    mkdir -p $DEST_DIR/android-project/app/libs/$ANDROID_ABI

    BUILD_DIR=`pwd`/../../build/$BUILD
    THIRD_DIR=`pwd`/../../third_party/usr/$BUILD

    cp -v $BUILD_DIR/libpitch-tracker.so $DEST_DIR/android-project/app/libs/$ANDROID_ABI
    cp -vL $THIRD_DIR/lib/libhidapi.so $DEST_DIR/android-project/app/libs/$ANDROID_ABI
    cp -vL $THIRD_DIR/lib/libSDL2.so $DEST_DIR/android-project/app/libs/$ANDROID_ABI
    cp -vL $THIRD_DIR/lib/libSDL2_ttf.so $DEST_DIR/android-project/app/libs/$ANDROID_ABI
    cp -vL $THIRD_DIR/lib/libSDL2_gfx.so $DEST_DIR/android-project/app/libs/$ANDROID_ABI
done

cp -v $SRC_DIR/Montserrat.ttf $DEST_DIR/android-project/app/src/main/assets

if [ -z "$1" ];
then
    cd $DEST_DIR/android-project && ./gradlew assembleRelease
    cp app/build/outputs/apk/release/app-release.apk ../../pitch-tracker-android.apk
else
    cd $DEST_DIR/android-project && ./gradlew "$1"
fi

