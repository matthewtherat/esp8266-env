# esp

```bash
sudo apt-get remove modemmanager
```

## Building the cross compile environment


### Clone and install the open-sdk

```bash
git clone --recursive https://github.com/pfalcon/esp-open-sdk.git
cd exp-open-sk
make
```

Create bash script to source and add the tool chain binaries into `PATH`

```bash
echo '''
#! /usr/bin/env bash
export PATH=$PWD/xtensa-lx106-elf/bin:$PATH
''' > activate.sh
chmod +x activate.sh
```

Source the newly created script update the `PATH` variable:

```bash
. activate.sh
```

