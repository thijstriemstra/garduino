Garduino
========

Arduino code for greenhouse monitoring using ESP32.

## Features

Controls 12V water valve and water pump and starts them once a day
to water the plants for a period of time. A water flow meter keeps
track of how much water is used.

Device status is displayed on a small 0.91 inch OLED screen.

Includes a manual mode button to enable/disable the water valve manually.

Time is stored in a battery-powered DS3231 realtime clock.

### Sensors

| Target | Sensor | Location | Amount |
| --- | --- | --- | --- |
| Light | BH1750 | Inside | 1 |
| Soil moisture | Capacitive Soil Moisture Sensor v2.0 | Inside | 5 |
| Temperature/pressure/humidity | BME280 | Inside | 1 |
| Air temperature | DS18B20 | Outside | 1 |
| Water temperature | DS18B20 | Outside | 1 |
| Rain | YL-83 | Outside | 1 |

Readings from the sensors are sent to a MQTT server over WiFi (if available).

## Wiring

### Lolin32 Lite

| Device | Pin Label | ESP32 Pin | Type |
| --- | --- | --- | --- |
| TCA9548A I2C Expander | SDA | `4` | I2C bus 1 (address: `0x70`) |
| TCA9548A I2C Expander | SCL | `22` | I2C bus 1 (address: `0x70`) |
| DS3231 Realtime Clock | SDA | `16` | I2C bus 0 (address: `0x68`) |
| DS3231 Realtime Clock | SCL | `17` | I2C bus 0 (address: `0x68`) |
| AT24C32 EEPROM on DS3231 | SDA | `16` |  I2C bus 0 (address: `0x57`) |
| AT24C32 EEPROM on DS3231 | SCL | `17` |  I2C bus 0 (address: `0x57`) |
| MCP3008 Analog Expander | CLK | `18` | SCK |
| MCP3008 Analog Expander | DOUT | `19` | MISO |
| MCP3008 Analog Expander | DIN | `23` | MOSI |
| MCP3008 Analog Expander | CS | `5` | SS |
| DS18B20 Temperature Sensors | GPIO | `13` | Digital Input |
| Single Channel Relay | GPIO | `2` | Digital Output |
| Water Flow Sensor | GPIO | `15` | Analog Input |
| Manual Run Button | GPIO | `36` | Digital Input |
| Power Button | GPIO | `39` | Digital Input |
| Watering Indication LED | GPIO | `25` | Digital Output |
| Manual Run LED | GPIO | `26` | Digital Output |
| Network LED | GPIO | `33` | Digital Output |
| Power LED | GPIO | `27` | Digital Output |

### TCA9548A

| Device | Pin Label | TCA9548A Pin | Address |
| --- | --- | --- | --- |
| BH1750 light sensor | SDA | `SD0` | `0x23` |
| BH1750 light sensor | SCL | `SC0` | `0x23` |
| BME280 barometer | SDA | `SD1` | `0x76` |
| BME280 barometer | SCL | `SC1` | `0x76` |
| 0.96" OLED Display | SDA | `SD2` | `0x3C` |
| 0.96" OLED Display | SCL | `SC2` | `0x3C` |

### MCP3008

| Device | Type | MCP3008 Pin |
| --- | --- | --- |
| Capacitive Soil 1 | Analog Input | `0` |
| Capacitive Soil 2 | Analog Input | `1` |
| Capacitive Soil 3 | Analog Input | `2` |
| Capacitive Soil 4 | Analog Input | `3` |
| Capacitive Soil 5 | Analog Input | `4` |
| YL-83 Rain Sensor | Analog Input | `5` |

## 3D Printable Parts

| Part | Amount |
| --- | --- |
| [Buck converter bottom](cad/buck-converter-bottom.stl) | 1 |
| [Buck converter top](cad/buck-converter-top.stl) | 1 |
| [Buck converter icon](cad/buck-converter-icon.stl) | 1 |
| [Buck converter latch](cad/buck-converter-latch.stl) | 2 |
| [Main panel top]((cad/Main-panel-top.stl)) | 1 |
| [Main panel bottom]((cad/Main-panel-bottom.stl)) | 1 |
| [Main panel clamp]((cad/Main-panel-clamp.stl)) | 1 |
| [Main panel led cover]((cad/Main-panel-le-cover.stl)) | 1 |
| [Main panel button]((cad/Main-panel-button.stl)) | 2 |
| [Main panel button mount]((cad/Main-panel-button-mount.stl)) | 1 |
| [Main panel manual icon]((cad/Main-panel-manual-icon.stl)) | 1 |
| [Main panel sleep icon]((cad/Main-panel-sleep-icon.stl)) | 1 |
| [BME280 Cover](https://www.printables.com/model/129341-bme280-hiletgo-33v-atmospheric-pressure-sensor-cov) | 1 |

## Output

Example of cycle report on serial line:

```
========================
  = garduino v3.0.0 =
========================

Board:          WEMOS LOLIN32 Lite
Local time:     2021-05-19 01:53:16
***********************************
Wakeup reason: timer (60 sec)
***********************************
WiFi - SSID: snowblower
WiFi - Connecting...
WiFi - Connected.
IP address: 10.42.1.158
==============================
MQTT - Connecting to 10.42.0.1
Connected to MQTT.
==============================

************************************
      Watering: No
        Period: 120 sec
Daily schedule: 19:00:00
  Current time: 01:53:16
      Last run: 2021-05-19 19:00:59
************************************

MQTT - Publishing sensor data...

Inside
------

Light:                  7.50 lx
Humidity:               46.04%
Temperature:            23.68 °C
Pressure:               1015.73 hPa
Soil-1 moisture:        51%
Soil-2 moisture:        50%
Soil-3 moisture:        48%
Soil-4 moisture:        51%
Soil-5 moisture:        51%
System:                 24.00 °C

Outside
-------

Rain:                   0%
Temperature:            21.62 °C

Water
-------

Temperature:            21.19 °C
Current:                0.00 ltr
Total:                  40.71 ltr

**********************************************

******************************
**  Going to sleep... Bye.  **
******************************
```

## Development

Checkout code and open the `garduino` project in VSCode with the PlatformIO
IDE extension.

## Local NTP Server

Setup a local NTP server instead of fetching this information online every
time.

```console
sudo apt-get install ntp
```

Verify installation:

```console
sntp --version
```

Switch the server pool to the ones closest to your location. Go to
https://support.ntp.org/bin/view/Servers/NTPPoolServers and find the
nearest pool, e.g. Europe. Update `/etc/ntp.conf` accordingly:

```
	   server 0.europe.pool.ntp.org
	   server 1.europe.pool.ntp.org
	   server 2.europe.pool.ntp.org
	   server 3.europe.pool.ntp.org
```

Restart the server:

```console
sudo service ntp restart
```

Check the status:

```console
 sudo service ntp status
 ```

You can now use this machine for NTP time synchronization on the ESP32.
