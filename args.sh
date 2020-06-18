#!/bin/bash

BUILD=$1

case $BUILD in
    win32)
        CROSS=i686-w64-mingw32.shared-
        ;;
    win64)
        CROSS=x86_64-w64-mingw32.shared-
        ;;
    *)
        BUILD=linux
        CROSS=
        ;;
esac

