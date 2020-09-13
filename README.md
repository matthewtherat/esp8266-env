# esp8266 development environment

```bash
sudo apt-get remove modemmanager
sudo apt-get install python2.7 python3-pip screen
sudo -H pip3 install --upgrade pip setuptools wheel
```

### Clone and install

```bash
git clone --recursive git@github.com:pylover/esp8266-env.git 
cd esp8266-env
./setup.sh
```

### Hello World!

First, build and flash the FOTA image.
```bash
cd esp8266-env
source activate.sh
cd fota
make assets_map6user1
make flash_map6user1
```

Then search for WIFI access point named something like `NewDevice_xxxxxxxxxxx`
, connect and open the address http://192.168.43.1 to configure the newly 
cooked device. Then press reboot.

Use `avahi-browse` to find the new device's IP Address:

```bash
avahi-browse -a
```

Output example:

```
E Ifce Prot Name                                          Type                 Domain
+   wlo1 IPv4 tvcontroller                                  _ESPWebAdmin._tcp    local
=   wlo1 IPv4 tvcontroller                                  _ESPWebAdmin._tcp    local
   hostname = [tvcontroller.local]
   address = [192.168.8.150]
   port = [80]
   txt = ["version = 0.1.0" "vendor = Espressif"]
: Cache exhausted
```

Now, transfer the `helloworld` firmware over the air using:

```bash
cd ../helloworld
make map6user2
make fota HOST=192.168.8.150
```



### SDK Examples

```bash
cd esp8266-env
source activate.sh

cd sdk/examples/simple_pair
./gen_misc.sh

```
