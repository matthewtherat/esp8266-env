#! /usr/bin/env bash

if [ -z $1 ]; then
  read -p "Enter project name (will be created by script): " DEST
else
  DEST=$1
fi


DEST=`readlink -f $DEST`
PRJ=`basename $DEST`
SRC=`readlink -f $SDK_PATH/../fota`

set -e

function preparedestination () {
  echo "Prepare destination"
  if [ -d "$DEST" ]; then
    read -p "Directory $DEST already exists. Overwrite? [NO/yes] " overwrite
    if [ "$overwrite" != "yes" ]; then
      return 1
    fi
    echo "Overwriting directory $DEST."
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
  read -p "[1/2]? " proto

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
  git -C $DEST submodule add --force --depth 1 ${url}/esp8266-debug.git debug
  git -C $DEST submodule add --force --depth 1 ${url}/esp8266-ringbuffer.git \
    ringbuffer
}


function cleanup () {
  echo "Remove unneeded directories"
  rmdir $DEST/debug
  rmdir $DEST/http
  rmdir $DEST/httpd
  rmdir $DEST/ringbuffer
  rmdir $DEST/uns

  echo "Remove fota lines"
  sed -i "s|#define __name__.*|#define __name__ \"${PRJ}\"|g" \
    $DEST/include/user_config.h

  sed -i "s|#define __version__.*|#define __version__ \"0.1.0\"|g" \
    $DEST/include/user_config.h
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

cd <project>
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

echo "Project Name: ${PRJ}"
echo "Destination: ${DEST}"
echo "Source: ${SRC}"

copyfota
cleanup
updatereadme
addsubmodules

echo "Compile"

cd $DEST

echo "Choose spi size and map to compile:"
echo "  2: 1024KB( 512KB+ 512KB)"
echo "  4: 4096KB( 512KB+ 512KB)"
echo "  6: 4096KB(1024KB+1024KB)"
read -p "enter (2/4/6, default 6): " SPIMAP

if [ -z $SPIMAP ]; then
  SPIMAP=6
fi


make map${SPIMAP}user1

