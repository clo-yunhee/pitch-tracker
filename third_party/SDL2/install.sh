#!/bin/bash

DIR=`pwd`

cd SDL2/build/org.libsdl/app

for target in android-arm android-arm64 android-x86 android-x86_64;
do
    source $DIR/../../args.sh $target

    INSDIR=$DIR/../usr/$BUILD

    mkdir -p $INSDIR/lib
    cp libs/$ANDROID_ABI/* $INSDIR/lib

    mkdir -p $INSDIR/include/SDL2
    cp jni/SDL/include/* $INSDIR/include/SDL2
    cp jni/*/SDL*.h $INSDIR/include/SDL2

    mkdir -p $INSDIR/lib/pkgconfig

    cat <<'EOF' > $INSDIR/lib/pkgconfig/sdl2.pc
prefix=$INSDIR
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include
Name: sdl2
Description: Simple DirectMedia Layer is a cross-platform multimedia library designed to provide low level access to audio, keyboard, mouse, joystick, 3D hardware via OpenGL, and 2D video framebuffer.
Version: 2.0.12
Requires:
Conflicts:
Libs: -L${libdir} -lSDL2
Cflags: -I${includedir}/SDL2   -D_REENTRANT
EOF

    cat <<'EOF' > $INSDIR/lib/pkgconfig/SDL2_ttf.pc
prefix=$INSDIR
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include
Name: SDL2_ttf
Description: ttf library for Simple DirectMedia Layer with FreeType 2 support
Version: 2.0.15
Requires: sdl2 >= 2.0.0
Libs: -L${libdir} -lSDL2_ttf
Cflags: -I${includedir}/SDL2
EOF

done
