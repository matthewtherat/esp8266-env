#! /usr/bin/env bash

if [ -z $1 ]; then
  read -p "Project directory (will be created by script): " DEST
else
  DEST=$1
fi

SRC=`readlink -f $SDK_PATH/../fota`

set -e

function copyfota () {
  echo "Copy initial files"
  if [ -d "$DEST" ]; then
    read -p "Directory $DEST already exists. delete it [NO/yes]? " deldest
    if [ "$deldest" != "yes" ]; then
      >&2 echo "Directory $DEST is already exists. Aborting."
      return 1
    fi
    rm -rf $DEST
  fi
  mkdir -p $DEST
  git -C $SRC archive --format=tar HEAD | tar -xvC $DEST/
}


function addsubmodules () {
  echo "Initialize git repo"
  git -C $DEST init

  echo "Add submodules"
  git -C $DEST submodule add --depth 1 \
    https://github.com/pylover/esp8266-httpserver.git \
    httpd

  git -C $DEST submodule add --depth 1 \
    https://github.com/pylover/esp8266-unslib.git \
    uns

  git -C $DEST submodule add --depth 1 \
    https://github.com/pylover/esp8266-httpclient.git \
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


copyfota
cleanup
updatereadme
addsubmodules

echo "Compile"

cd $DEST
make map6user1

