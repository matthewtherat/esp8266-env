#! /usr/bin/env bash

# Build toolchain
cd xtensa-toolchain/release
make linux64
cd ../../

# Install esptool
sudo -H pip3 install esptool

# Build libhttpserver
cd sdk/httpserver
./make_lib.sh
cd ../../


