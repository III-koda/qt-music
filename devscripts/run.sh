#!/bin/bash

# Create a build directory and move into it
BUILD_DIR="build"
[ ! -d $BUILD_DIR ] && mkdir $BUILD_DIR
cd $BUILD_DIR

cmake .. && ninja && ./qt-music-player
