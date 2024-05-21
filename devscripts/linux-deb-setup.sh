#!/bin/bash

WORKDIR=$(pwd);

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

echo "Installing Qt5..."
sudo apt-get update;
sudo apt-get install -y qtbase5-dev;

cd $WORKDIR;
cmake --preset=default;
