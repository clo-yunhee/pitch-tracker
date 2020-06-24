#!/bin/bash

rm -f eigen-3.3.7.tar.gz
wget https://gitlab.com/libeigen/eigen/-/archive/3.3.7/eigen-3.3.7.tar.gz
tar xvf eigen-3.3.7.tar.gz
cd eigen-3.3.7
mkdir -p build
cd build

for target in android-arm android-arm64 android-x86 android-x86_64;
do
    mkdir -p $target
    cd $target

    cmake -DCMAKE_INSTALL_PREFIX=$(readlink -f "../../../../usr/$target") ../../ 
    make install

    cd ..
done
