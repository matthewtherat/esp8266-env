#! /usr/bin/env bash

cd xtensa-toolchain/release
make linux64
cd ../../
sudo -H pip3 install esptool

