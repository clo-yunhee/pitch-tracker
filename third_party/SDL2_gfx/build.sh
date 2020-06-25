#!/bin/bash

rm -f SDL2_gfx-1.0.4.tar.gz
wget http://www.ferzkopp.net/Software/SDL2_gfx/SDL2_gfx-1.0.4.tar.gz
tar xvf SDL2_gfx-1.0.4.tar.gz

cp -f ../fftw3/fftw-3.3.8/{config.guess,config.sub} SDL2_gfx-1.0.4

cd SDL2_gfx-1.0.4

./autogen.sh

mkdir -p build
cd build

for target in android-arm android-arm64 android-x86 android-x86_64;
do
    source ../../../../args.sh $target

    mkdir -p $target
    cd $target

    PREFIX=$(readlink -f "../../../../usr/$target")

    ../../configure --host=$HOST --prefix=$PREFIX --enable-shared --disable-static --disable-mmx --with-sdl-prefix=$PREFIX
    make -j$(nproc)
    make install

    cd ..
done

