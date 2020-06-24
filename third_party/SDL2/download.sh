#!/bin/bash

ver=2.0.12
ttf_ver=2.0.15

wget https://www.libsdl.org/release/SDL2-$ver.tar.gz
wget https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-$ttf_ver.tar.gz

tar xf SDL2-$ver.tar.gz
tar xf SDL2_ttf-$ttf_ver.tar.gz

ln -s SDL2-$ver SDL2
ln -s SDL2_ttf-$ttf_ver SDL2_ttf

