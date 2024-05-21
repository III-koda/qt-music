#!/bin/bash

WORKDIR=$(pwd);

if [ -d "build" ]; then
    echo -e "\e[33mYou seems to have an existing build directory...";
    echo -e "We recommend remove the build directory and do a clean build if this script fails\e[0m";
fi

if [ ! -d "$HOME/vcpkg" ]; then
    echo "Installing VCPKG..."
    cd $HOME;
    git clone https://github.com/microsoft/vcpkg.git vcpkg;
    cd vcpkg;
    ./bootstrap-vcpkg.sh;
fi

if [[ $SHELL == *"csh"* ]]; then
    RCFILE=".cshrc";
elif [[ $SHELL == *"ksh"* ]]; then
    RCFILE=".kshrc";
elif [[ $SHELL == *"zsh"* ]]; then
    RCFILE=".zshrc";
else
    RCFILE=".bashrc";
fi
echo -e 'VCPKG_ROOT="$HOME/vcpkg"' >> $HOME/$RCFILE
echo -e 'alias vcpkg="$HOME/vcpkg/vcpkg"' >> $HOME/$RCFILE;

export VCPKG_ROOT="$HOME/vcpkg";
alias vcpkg="$HOME/vcpkg/vcpkg";

sudo apt-get update;
sudo apt-get install -y ninja-build;
# Building Qt with a package manager seems to be a pain (at least in Linux).
# It takes a very very long time and it is reported to have a lot of issues
# as well. So we will install it with apt-get instead.
sudo apt-get install -y qtbase5-dev;

cd $WORKDIR;
cmake --preset=default;
