#!/bin/bash

rm -f fftw-3.3.8.tar.gz
wget http://www.fftw.org/fftw-3.3.8.tar.gz
tar xvf fftw-3.3.8.tar.gz
cd fftw-3.3.8
mkdir -p build
cd build

for target in android-arm android-arm64 android-x86 android-x86_64;
do
    source ../../../../args.sh $target

    mkdir -p $target
    cd $target

    ../../configure --host=$HOST --prefix=$(readlink -f "../../../../usr/$target") 
    make -j$(nproc)
    make install

    cd ..
done
