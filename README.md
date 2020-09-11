# esp8266 development environment

```bash
sudo apt-get remove modemmanager
sudo apt-get install python2.7 python3-pip
sudo -H pip3 install --upgrade pip setuptools wheel
```

### Clone and install

```bash
git clone --recursive git@github.com:pylover/esp8266-env.git 
cd esp8266-env
./setup.sh
```

### Hello World!

```bash
cd helloworld
make map6user2
```


### SDK Examples

```bash
cd esp8266-env
source activate.sh

cd sdk/examples/simple_pair
./gen_misc.sh

```
