# esp

```bash
sudo apt-get remove modemmanager
```

## Building the cross compile environment


### Clone and install

```bash
git clone --recursive git@github.com:pylover/esp.git
cd esp/xtensa-toolchain/release
make linux64
mv xtensa-toolchain/xtensa-lx106-elf/ ../../
```

