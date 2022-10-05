#!/bin/bash
echo "---- updating submodule ----"
git submodule update --init

echo "---- making build directory ----"
mkdir build

echo "---- change work dir to build ----"
cd build

echo "---- Check if ninja build is available ----"

if ! command -v ninja &> /dev/null; then
   cmake .. -DCMAKE_BUILD_TYPE=Release
else
   cmake .. -DCMAKE_BUILD_TYPE=Release -GNinja
fi

echo "---- Building ----"
if ! command -v ninja &> /dev/null; then
   make
else
   ninja
fi

echo "---- Going back to source dir ----"
cd ..
