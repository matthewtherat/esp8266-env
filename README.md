# esp8266 development environment

```bash
sudo apt-get remove modemmanager
sudo apt-get install python2.7 python3-pip screen qpdf
sudo -H pip3 install --upgrade pip setuptools wheel
```

### Clone and install

```bash
git clone --recursive git@github.com:pylover/esp8266-env.git 
cd esp8266-env
scripts/setup.sh
```

### Hello World!

First, build and flash the FOTA image.
```bash
cd esp8266-env
source activate.sh
cd fota
make cleanup_map6user1_params
make assets_map6user1
make flash_map6user1
```

You may use `make screen` to view esp debug console. use `CTRL+A` then `k` to
exit.

Then search for WIFI access point named something like `NewDevice_xxxxxxxxxxx`
, connect and open the address http://192.168.43.1 to configure the newly 
cooked device. Then press reboot.

Use `uns resolve` to find the new device's IP Address:

```bash
uns d home.NewDevice
```

Output example:

```
Sending home. to 224.0.0.70:5333
192.168.8.162:5333: home.NewDevice
```

Now, create and transfer the `helloworld` firmware over the air using:

```bash
esp8266-newproject helloworld
cd helloworld
make map6user2
make fota HOST=$(uns d -s home.NewDevice)
```

or edit the `Makefile` and set `HOST ?= home.newDevice` to make it simpler.


### SDK Examples

```bash
cd esp8266-env
source activate.sh

cd sdk/examples/simple_pair
./gen_misc.sh

```
