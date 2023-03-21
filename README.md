Garduino
========

Greenhouse monitoring using ESP32.

## Features

- Controls 24V water waterpump and starts it once a day to water the plants for
  a period of time
- Device status is displayed on a small 0.91 inch OLED screen
- Includes a manual mode button to enable/disable the water valve manually
- Time is stored in a battery-powered DS3231 realtime clock

### Sensors

| Target | Sensor | Location | Amount |
| --- | --- | --- | --- |
| Light | BH1750 | Inside | 1 |
| Soil moisture | Capacitive Soil Moisture Sensor v2.0 | Inside | 8 |
| Temperature/pressure/humidity | BME280 | Inside | 1 |
| Air temperature | DS18B20 | Outside | 1 |
| Water temperature | DS18B20 | Outside | 1 |

Readings from the sensors are sent to a MQTT server over WiFi (if available).

## Wiring

### AZ-Delivery Devkit V4

| Device | Pin Label | ESP32 Pin | Type |
| --- | --- | --- | --- |
| TCA9548A I2C Expander | SDA | `4` | I2C bus 0 (address: `0x70`) |
| TCA9548A I2C Expander | SCL | `22` | I2C bus 0 (address: `0x70`) |
| DS3231 Realtime Clock | SDA | `4` | I2C bus 0 (address: `0x68`) |
| DS3231 Realtime Clock | SCL | `22` | I2C bus 0 (address: `0x68`) |
| AT24C32 EEPROM on DS3231 | SDA | `4` |  I2C bus 0 (address: `0x57`) |
| AT24C32 EEPROM on DS3231 | SCL | `22` |  I2C bus 0 (address: `0x57`) |
| PCF8574 Digital Expander | SDA | `4` |  I2C bus 0 (address: `0x20`) |
| PCF8574 Digital Expander | SCL | `22` |  I2C bus 0 (address: `0x20`) |
| MCP3008 Analog Expander | CLK | `18` | SCK |
| MCP3008 Analog Expander | DOUT | `19` | MISO |
| MCP3008 Analog Expander | DIN | `23` | MOSI |
| MCP3008 Analog Expander | CS | `5` | SS |
| DS18B20 Temperature Sensors | GPIO | `13` | Digital Input |
| Buzzer | GPIO | `25` | Digital Output |
| Single Channel Relay | GPIO | `26` | Digital Output |
| Manual Run Button | GPIO | `39` | Digital Input |
| Power Button | GPIO | `36` | Digital Input |

### TCA9548A

| Device | Pin Label | TCA9548A Pin | Address |
| --- | --- | --- | --- |
| BH1750 light sensor | SDA | `SD0` | `0x23` |
| BH1750 light sensor | SCL | `SC0` | `0x23` |
| BME280 barometer | SDA | `SD1` | `0x76` |
| BME280 barometer | SCL | `SC1` | `0x76` |
| 0.91" OLED Display | SDA | `SD2` | `0x3C` |
| 0.91" OLED Display | SCL | `SC2` | `0x3C` |

### PCF8574

| Device | Type | PCF8574 Pin |
| --- | --- | --- |
| Watering Indication LED (White) | Digital Output | `0` |
| Manual Run LED (Yellow) | Digital Output | `2` |
| Network LED (Blue) | Digital Output | `1` |
| Power LED (Red) | Digital Output | `3` |

### MCP3008

| Device | Type | MCP3008 Pin |
| --- | --- | --- |
| Capacitive Soil 1 | Analog Input | `0` |
| Capacitive Soil 2 | Analog Input | `1` |
| Capacitive Soil 3 | Analog Input | `2` |
| Capacitive Soil 4 | Analog Input | `3` |
| Capacitive Soil 5 | Analog Input | `4` |
| Capacitive Soil 6 | Analog Input | `5` |
| Capacitive Soil 7 | Analog Input | `6` |
| Capacitive Soil 8 | Analog Input | `7` |

## 3D Printable Parts

| Part | Amount |
| --- | --- |
| [Buck converter bottom](cad/buck-converter-bottom.stl) | 1 |
| [Buck converter top](cad/buck-converter-top.stl) | 1 |
| [Buck converter icon](cad/buck-converter-icon.stl) | 1 |
| [Buck converter latch](cad/buck-converter-latch.stl) | 2 |

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
