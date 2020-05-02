py-garden
=========

Arduino code for greenhouse monitoring using ESP32.

## Features

Controls 12V water valve and 12V water pump and starts them once a day
to water the plants for a period of time.

Also includes a manual mode controlled by a button for extra watering.

Sensors:

 - Soil (2 FC-28 sensors)
 - Temperature inside/outside/water (BME280 and 2 DS18B20 sensors)
 - Humididity and pressure (BME280 sensor)
 - Rain (YL-83 sensor)
 - Light (BH1750 sensor)

Readings from the sensors are sent to a MQTT server over WiFi (when available).
Data and time is stored in a battery-powered DS3231 realtime clock. Clock is
synced with NTP server once a day.

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
| Dual DS18B20 Temperature Sensors | 15 |
| YL-83 Rain Sensor | 34 |
| FC-38 Soil Sensor 1 | 35 |
| FC-38 Soil Sensor 2 | 32 |
| BME280 Barometer SDA | 0 |
| BME280 Barometer SCL | 27 |
| DS3231 Clock SDA | 17 |
| DS3231 Clock SCL | 5 |

## Output

Example of cycle report:

```
Build date: 2020/05/02 14:54:59
Local time: 2020/05/02 15:16:47
******************************
Wakeup cause: timer
Manual mode: 0
Schedule: wake up every 40 sec
******************************
WiFi - Connecting to MySSID
==============================
WiFi connected.
IP address: 10.42.0.158
==============================
MQTT - Connecting to 10.42.0.1
Connected to MQTT.
==============================

************************************
      Watering: No
        Period: 120 sec
Daily schedule: 14:00:00
  Current time: 15:16:47
      Last run: 2020/05/02 14:00:19
************************************

Publishing sensor data...

Inside
------

Light:                  160.83 lx
Temperature:            19.12 °C
Pressure:               1008.86
Humidity:               46.13%
Soil-1 moisture:        100% dry
Soil-2 moisture:        100% dry
System:                 20.25 °C

Outside
-------

Rain:                   100% dry
Temperature air:        -127.00 °C
Temperature water:      -127.00 °C

**********************************************

******************************
**  Going to sleep... Bye.  **
******************************
```

## Development

Checkout code and open the `pygarden` project in VSCode with the PlatformIO IDE extension.

## Local NTP Server

Setup a local NTP server instead of fetching this information online every time.

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