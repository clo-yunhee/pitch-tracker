#!/bin/bash

rm -f pkg-config-0.29.2.tar.gz
wget https://pkg-config.freedesktop.org/releases/pkg-config-0.29.2.tar.gz
tar xvf pkg-config-0.29.2.tar.gz
cd pkg-config-0.29.2
mkdir -p build
cd build

for target in android-arm android-arm64 android-x86 android-x86_64;
do
    mkdir -p $target
    cd $target

    ../../configure --prefix=$(readlink -f "../../../../usr/$target") --with-internal-glib 
    make -j$(nproc)
    make install

    cd ..
done
