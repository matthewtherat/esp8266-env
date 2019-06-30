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
```

### How to use

```bash
source activate.sh

cd sdk
cp -r exampls/simple_pair .
cd simple_pair 
./gen_misc.sh


```
