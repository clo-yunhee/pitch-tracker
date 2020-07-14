#!/bin/bash

set -e

tmp=/AppDir

mkdir -p $tmp/usr/bin
mkdir -p $tmp/usr/lib
mkdir -p $tmp/usr/share/applications
mkdir -p $tmp/usr/share/icons/hicolor/128x128

cp -v /build/pitch-tracker $tmp/usr/bin
cp -v $(ldd /build/pitch-tracker | grep -o '\W/[^ ]*') $tmp/usr/lib || true
cp -v /src/dist-res/pitch-tracker.desktop $tmp/usr/share/applications
cp -v /src/dist-res/pitch-tracker.png $tmp/usr/share/icons/hicolor/128x128
ln -sfv usr/bin/pitch-tracker $tmp/AppRun
ln -sfv usr/share/applications/pitch-tracker.desktop $tmp/pitch-tracker.desktop
ln -sfv usr/share/icons/hicolor/128x128/pitch-tracker.png $tmp/pitch-tracker.png
cp -v /src/Montserrat.ttf $tmp

cd /dist && appimagetool --appimage-extract-and-run /AppDir pitch-tracker.AppImage

