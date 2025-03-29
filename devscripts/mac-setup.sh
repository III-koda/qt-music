#!/bin/zsh

WORKDIR=$(pwd);

if [ -d "build" ]; then
    echo -e "\e[33mYou seems to have an existing build directory...";
    echo -e "We recommend remove the build directory and do a clean build if this script fails\e[0m";
fi

brew install vcpkg;
if [ $? -ne 0 ]; then
    echo "\e[33mFailed to install vcpkg\e[0m";
    exit 1;
fi
git clone https://github.com/microsoft/vcpkg "$HOME/vcpkg"
export VCPKG_ROOT="$HOME/vcpkg"

brew install qt@5;
if [ $? -ne 0 ]; then
    echo "\e[33mFailed to install Qt\e[0m";
    exit 1;
fi
export PATH="/opt/homebrew/opt/qt@5/bin:$PATH";
export LDFLAGS="-L/opt/homebrew/opt/qt@5/lib";
export CPPFLAGS="-I/opt/homebrew/opt/qt@5/include";
export PKG_CONFIG_PATH="/opt/homebrew/opt/qt@5/lib/pkgconfig";

cd $WORKDIR;
cmake --preset=default;
