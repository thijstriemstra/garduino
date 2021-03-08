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

| Sensor | ESP32 Pin |
| --- | --- |
| Network LED | 4 |
| Power LED | 14 |
| Power Button | 18 |
| Manual Run LED | 2 |
| Manual Run Button | 13 |
| Watering Indication LED | 12 |
| BH1750 Light Sensor SDA | 23 |
| BH1750 Light Sensor SCL | 19 |
| Single Channel Relay | 22 |
| Water Flow Sensor | 16 |
| Dual DS18B20 Temperature Sensors | 15 |
| BME280 Barometer SDA | 0 |
| BME280 Barometer SCL | 27 |
| DS3231 Clock SDA | 17 |
| DS3231 Clock SCL | 5 |
| SSD1306 128x32 OLED SDA | 26 |
| SSD1306 128x32 OLED SCL | 25 |
| Capacitive Soil Sensor 1 | 39 |
| Capacitive Soil Sensor 2 | 32 |
| Capacitive Soil Sensor 3 | 34 |
| Capacitive Soil Sensor 4 | 36 |
| Capacitive Soil Sensor 5 | 33 |
| YL-83 Rain Sensor | 35 |

## Output

Example of cycle report on serial line:

```
garduino 2.3.0
Build date: 2020/05/02 16:30:31
Local time: 2020/05/02 16:44:03
******************************
Wakeup reason: timer (40 sec)
******************************
WiFi - SSID: MyInternet
WiFi - Connecting...
WiFi - Connected.
IP address: 10.42.0.158
==============================
MQTT - Connecting to 10.42.0.1
Connected to MQTT.
==============================

************************************
      Watering: No
        Period: 180 sec
Daily schedule: 14:00:00
  Current time: 16:44:03
      Last run: 2020/05/02 14:00:19
************************************

MQTT - Publishing sensor data...

Inside
------

Light:                  120.83 lx
Humidity:               45.62%
Temperature:            22.09 °C
Pressure:               1018.10 hPa
Soil-1 moisture:        33%
Soil-2 moisture:        51%
System:                 22.00 °C

Outside
-------

Rain:                   0%
Temperature:            19.45 °C

Water
-------

Temperature:            16.12 °C
Current:                0.00 ltr
Total:                  40.21 ltr

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
