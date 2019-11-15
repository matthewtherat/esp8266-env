# esp

```bash
sudo apt-get remove modemmanager
```

## Building the cross compile environment


### Clone and install

```bash
git clone --recursive git@github.com:pylover/esp8266-env.git myenv
cd myenv
./setup.sh
```

### How to use

```bash
cd myenv
source activate.sh

cd sdk/examples/simple_pair
./gen_misc.sh


```
