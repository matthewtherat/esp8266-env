#! /usr/bin/env bash

SRC=`readlink -f $SDK_PATH/../fota`
DEST=$1

set -e

function copyfota () {
  if [ -d "$DEST" ]; then
    read -p "Directory $DEST already exists. delete it [NO/yes]? " deldest
    if [ "$deldest" != "yes" ]; then
      >&2 echo "Directory $DEST is already exists. Aborting."
      exit
    fi
    rm -rf $DEST
  fi
  mkdir -p $DEST
  git -C $SRC archive --format=tar HEAD | tar -xvC $DEST/
}


function removefotalines () {
  sed -i '/^#include "fotaweb.h"/d' $DEST/user/webadmin.c
  sed -i '/fotaweb_upgrade_firmware/d' $DEST/user/webadmin.c
  rm -rf $DEST/fota

  sed -i '/^\tfota/d' $DEST/Makefile
  sed -i '/^\tfota\/libfota.a/d' $DEST/Makefile
  sed -i '/\/fota\/include/d' $DEST/Makefile
  
  sed -i 's|Fota Image|Helloworld|g' $DEST/include/user_config.h
}


function updatereadme () {
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


echo "Copy initial files"

copyfota

echo "Remove fota lines"

removefotalines

echo "Updating readme file"

updatereadme

echo "done."

