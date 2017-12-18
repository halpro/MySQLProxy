#!/bin/bash
rm -rf build/release
mkdir build
mkdir build/release
cd build/release

cmake -D CMAKE_BUILD_TYPE=Release ../..
make -j8
