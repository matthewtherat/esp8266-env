#! /usr/bin/env bash

HERE=`dirname "$(readlink -f "$BASH_SOURCE")"`
ENVDIR=`readlink -f $HERE/..`
# Build toolchain
cd $ENVDIR/xtensa-toolchain/release
make linux64
cd ${ENVDIR}

# Install esptool
pip3 install esptool

# Install unslib command line utility
pip3 install 'uns >= 2'

