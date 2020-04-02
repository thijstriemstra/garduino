MicroPython scripts for garden monitoring using ESP32.

## Installation

Checkout code:

```console
git clone https://github.com/thijstriemstra/py-garden.git
cd py-garden/micropython
```

Create a virtual environment (using Python 3.4 or newer):

```console
mkvirtualenv pygarden
```

Install library and dependencies in the virtual environment:

```console
pip install -e .
```

## Development
-----------

Use the Thonny IDE for development. Start it from the Python virtual
environment:

```console
thonny
```

### Firmware

This project uses a fork of [MicroPython for ESP32 with psRAM support](https://github.com/loboris/MicroPython_ESP32_psRAM_LoBo).

Check their wiki for the [build instructions](https://github.com/loboris/MicroPython_ESP32_psRAM_LoBo/wiki/build).

### Configuration file

Example of configuration file:

```ini
[general]
auto_start = True
interval = 500
base_topic = mygarden

[network]
ssid = MyInternet
password = passw0rd

[log]
logfile = log.txt

[broker]
server = 192.168.2.12
user = mqtt-user
password = mqtt-password
device_id = my_garden1

[rtc]
enabled = True
hardware = True
scl_pin = 4
sda_pin = 0
timezone = Europe/Amsterdam

[temperature]
enabled = True
pin = 23
roms = 9f0317208374ff28,160317205a8eff28
9f0317208374ff28 = inside
160317205a8eff28 = outside

[light]
enabled = True
i2c_id = 0
scl_pin = 2
sda_pin = 15

[soil]
enabled = True
pin_nrs = 32,33

[rain]
enabled = False
pin_nrs = 35

[display]
enabled = False
type = tm1637
dio_pin = 19
clk_pin = 23

[switch]
enabled = False
left_pin = 34
right_pin = 35
```
