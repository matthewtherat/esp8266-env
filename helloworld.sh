#! /usr/bin/env bash

set -e

cd ~/tmp
esp8266-newproject helloworld
cd helloworld
make fota
