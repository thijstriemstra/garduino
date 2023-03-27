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
00:00:00.063  ========================
00:00:00.064  =    garduino v4.0.0   =
00:00:00.064  ========================
00:00:00.065  Board:       AZ-Delivery ESP-32 Dev Kit C V4
00:00:00.075  Arduino:     2.0.7
00:00:00.075  ESP-IDF:     v4.4.4
00:00:00.080  Local time:  17:46:17
00:00:00.485  *******************************************
00:00:00.486  Wakeup reason: timer (60 sec)
00:00:00.487  *******************************************
00:00:00.488  WiFi - SSID: Boat
00:00:00.499  WiFi - Connecting...
00:00:00.708  WiFi - Connected.
00:00:00.709  WiFi - Hostname: garduino
00:00:00.709  Wifi - IP address: 192.168.2.2
00:00:00.710  ================================
00:00:00.721  MQTT - Timeout set to 30 seconds
00:00:00.721  MQTT - Broker: 192.168.2.1:1883
00:00:00.722  MQTT - Connecting...
00:00:00.775  MQTT - Connected.
00:00:00.776  ================================
00:00:00.777
00:00:00.777  ************************************
00:00:00.778        Watering: No
00:00:00.788          Period: 60 sec
00:00:00.789  Daily schedule: 19:00:00
00:00:00.789    Current time: 17:46:17
00:00:00.791        Last run: 2023-03-21 19:08:11
00:00:00.802  ************************************
00:00:00.802
00:00:00.803  MQTT - Publishing sensor data...
00:00:00.813
00:00:00.814  Inside
00:00:00.814  ------
00:00:00.814
00:00:00.815  Light:            350.67 lx
00:00:00.821  Humidity:         44.5%
00:00:00.822  Temperature:      25.14 °C
00:00:00.822  Pressure:         1002.44 hPa
00:00:00.840  Soil-1 wet:       0%
00:00:00.841  Soil-2 wet:       0%
00:00:00.851  Soil-3 wet:       0%
00:00:00.852  Soil-4 wet:       0%
00:00:00.852  Soil-5 wet:       0%
00:00:00.852  Soil-6 wet:       0%
00:00:00.853  Soil-7 wet:       98%
00:00:00.863  Soil-8 wet:       0%
00:00:00.854  System:           23.75 °C
00:00:00.854
00:00:00.854  Outside
00:00:00.855  -------
00:00:00.865
00:00:00.962  Temperature:      17.75 °C
00:00:00.967
00:00:00.967  Water
00:00:00.967  -----
00:00:00.967
00:00:00.968  Temperature:      18.25 °C
00:00:00.968
00:00:00.968  WIFI
00:00:00.969  ----
00:00:00.979
00:00:00.981  RSSI:             -63 dBm
00:00:00.981
00:00:00.981  **********************************************
00:00:00.982
00:00:16.558  MQTT - Published 16 messages.
00:00:16.559  MQTT - Disconnecting...
00:00:16.863
00:00:16.864  ******************************
00:00:16.865  **  Going to sleep... Bye.  **
00:00:16.866  ******************************
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
