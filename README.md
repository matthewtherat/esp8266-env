# esp

```bash
sudo apt-get remove modemmanager
```

## Building the cross compile environment


### Clone and install

```bash
git clone --recursive git@github.com:easyqiot/esp-env.git
cd esp/xtensa-toolchain/release
make linux64
mv xtensa-toolchain/xtensa-lx106-elf/ ../../
```

### How to use

```bash
source esp-env/nonos/activate.sh

cd esp-env/nonos/sdk
cp -r exampls/simple_pair .
cd simple_pair 
bash gen_misc.sh


```
