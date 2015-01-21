#!/bin/bash

mkdir build
cd build

#generate cmake project in the "build" folder
cmake ..

#build it
make

#move to working data folder with resources
cd ../../data

#run executable
./../proj.cmake/build/Match3