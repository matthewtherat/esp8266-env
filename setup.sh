#! /usr/bin/env bash

# Build toolchain
cd xtensa-toolchain/release
make linux64
cd ../../

# Install esptool
pip3 install esptool

# Install unslib command line utility
pip3 install uns

# Build libhttpserver
cd sdk/httpserver
./make_lib.sh
cd ../../


