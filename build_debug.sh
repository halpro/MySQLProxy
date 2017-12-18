#!/bin/bash
rm -rf build/debug
mkdir build
mkdir build/debug
cd build/debug

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=On -DCMAKE_BUILD_TYPE=Debug ../..
make -j4
