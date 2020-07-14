#!/bin/bash

set -e

tmp=/pitch-tracker

mkdir -p $tmp

cp -v /build/pitch-tracker.exe $tmp
$MXE/tools/copydlldeps.sh \
        --infile /build/pitch-tracker.exe \
        --destdir $tmp/pitch-tracker \
        --recursivesrcdir $MXE/usr/${cross::-1} \
        --objdump $MXE/usr/bin/${cross}objdump \
        --copy
cp -v /src/Montserrat.ttf $tmp

cd /dist
zip -r pitch-tracker-$target.zip $tmp
