#! /usr/bin/env bash

if [ -z $1 ]; then
  read -p "Project directory (will be created by script): " DEST
else
  DEST=$1
fi

SRC=`readlink -f $SDK_PATH/../fota`

set -e

function preparedestination () {
  echo "Prepare destination"
  if [ -d "$DEST" ]; then
    read -p "Directory $DEST already exists. delete it [NO/yes]? " deldest
    if [ "$deldest" != "yes" ]; then
      return
    fi
    echo "Removing directory $DEST."
    rm -rf $DEST
  fi
  mkdir -p $DEST
}


function copyfota () {
  echo "Copy initial files"
  git -C $SRC archive --format=tar HEAD | tar -xvC $DEST/
}


function addsubmodules () {
  local proto
  local url

  if [ ! -d "$DEST/.git" ]; then
    echo "Initializing a new git repository."
    git -C $DEST init
  fi
  
  echo "Protocol to clone submodules:"
  echo "  1. https (default)"
  echo "  2. ssh"
  read -p "[1/2]?" proto

  if [ "$proto" == "1" ]; then
    url='https://github.com/pylover'
  else
    url='git@github.com:pylover'
  fi

  echo "Cloning submodules"
  git -C $DEST submodule add --force --depth 1 ${url}/esp8266-httpserver.git \
    httpd
  git -C $DEST submodule add --force --depth 1 ${url}/esp8266-unslib.git uns
  git -C $DEST submodule add --force --depth 1 ${url}/esp8266-httpclient.git \
    http
}


function cleanup () {
  echo "Remove unneeded directories"
  rmdir $DEST/httpd
  rmdir $DEST/http
  rmdir $DEST/uns
  rm -rf $DEST/fota

  echo "Remove fota lines"
  sed -i '/^#include "fotaweb.h"/d' $DEST/user/webadmin.c
  sed -i '/fotaweb_upgrade_firmware/d' $DEST/user/webadmin.c

  sed -i '/^\tfota/d' $DEST/Makefile
  sed -i '/^\tfota\/libfota.a/d' $DEST/Makefile
  sed -i '/\/fota\/include/d' $DEST/Makefile
  
  sed -i 's|Fota Image|Helloworld|g' $DEST/include/user_config.h
  sed -i 's|REBOOTDELAY\s\+[0-9]\+|REBOOTDELAY    0|g' \
    $DEST/include/user_config.h

  sed -i 's|{"APP",|{"FOTA",|g' $DEST/user/webadmin.c
}


function updatereadme () {
  echo "Updating readme file."

  echo '
# Helloword 

### Build

Follow [this](https://github.com/pylover/esp8266-env) instruction 
to setup your environment.

```bash
cd esp8266-env
source activate.sh
cd ..

cd $DEST 
bash gen_misc.sh
```

Or use predefined make macros:

```bash
make clean
make assets_map6user1
make flash_map6user1 

```
' > $DEST/README.md

}

preparedestination
copyfota
cleanup
updatereadme
addsubmodules

echo "Compile"

cd $DEST

echo "Choose spi size and map to compile:"
echo "  2: 1024KB( 512KB+ 512KB)"
echo "  6: 4096KB(1024KB+1024KB)"
read -p "enter (2/6, default 6): " SPIMAP

if [ -z $SPIMAP ]; then
  SPIMAP=6
fi


make map${SPIMAP}user1

