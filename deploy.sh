#!/bin/bash

set -e
source ./args.sh

# Make sure it's built first.
./build.sh $BUILD

BUILD_DIR=`pwd`/build/$BUILD
DEST_DIR=`pwd`/dist/$BUILD

rm -rf $DEST_DIR
mkdir -p $DEST_DIR
cd $DEST_DIR

function distribute_windows()
{
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
}

function distribute_linux()
{
    mkdir -p $DEST_DIR/usr/bin
    mkdir -p $DEST_DIR/usr/lib
    mkdir -p $DEST_DIR/usr/share/applications
    mkdir -p $DEST_DIR/usr/share/icons/hicolor/128x128

    cp -v $DEST_DIR/../res/AppRun $DEST_DIR
    cp -v $BUILD_DIR/pitch-tracker $DEST_DIR/usr/bin
    cp -v $(ldd $BUILD_DIR/pitch-tracker | grep -o '\W/[^ ]*') $DEST_DIR/usr/lib
    cp -v $DEST_DIR/../res/pitch-tracker.desktop $DEST_DIR/usr/share/applications
    cp -v $DEST_DIR/../res/pitch-tracker.png $DEST_DIR/usr/share/icons/hicolor/128x128
    ln -rsfv $DEST_DIR/usr/share/applications/pitch-tracker.desktop $DEST_DIR/pitch-tracker.desktop
    ln -rsfv $DEST_DIR/usr/share/icons/hicolor/128x128/pitch-tracker.png $DEST_DIR/pitch-tracker.png
    cp -v $BUILD_DIR/../../Montserrat.ttf $DEST_DIR/usr

    cd $DEST_DIR/.. && appimagetool linux pitch-tracker-$BUILD.AppImage
}

case $BUILD in
    win32|win64)
        distribute_windows
        ;;
    linux)
        distribute_linux
        ;;
esac
