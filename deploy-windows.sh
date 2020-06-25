#!/bin/bash

set -e

DIR=`pwd`

for target in win32 win64;
do
    cd $DIR

    source ./args.sh $target

    # Make sure it's built first.
    ./build.sh $BUILD

    SRC_DIR=`pwd`
    BUILD_DIR=`pwd`/build/$BUILD
    DEST_DIR=`pwd`/dist/$BUILD

    rm -rf $DEST_DIR
    mkdir -p $DEST_DIR
    cd $DEST_DIR 

    mkdir -p $DEST_DIR/pitch-tracker

    cp -v $BUILD_DIR/pitch-tracker.exe $DEST_DIR/pitch-tracker
    $MXE/tools/copydlldeps.sh \
            --infile $BUILD_DIR/pitch-tracker.exe \
            --destdir $DEST_DIR/pitch-tracker \
            --recursivesrcdir $MXE/usr/${CROSS::-1} \
            --objdump $MXE/usr/bin/${CROSS}objdump \
            --copy
    cp -v $BUILD_DIR/../../*.ttf /$DEST_DIR/pitch-tracker

    cd $DEST_DIR
    zip -r ../pitch-tracker-$BUILD.zip pitch-tracker
done
