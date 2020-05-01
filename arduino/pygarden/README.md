py-garden
=========

Arduino code for greenhouse monitoring using ESP32.

## Features

Controls 12V water valve and 12V water pump and starts them once a day
to water the plants.

Sensors:

 - Soil (2 FC-28 sensors)
 - Temperature inside/outside/water (BME280 and 2 DS18B20 sensors)
 - Humididity and pressure (BME280 sensor)
 - Rain (YL-83 sensor)
 - Light (BH1750 sensor)

Readings from the sensors are sent to a MQTT server over WiFi.

## Wiring

| Sensor | ESP32 Pin |
| --- | --- |
| Network LED | 4 |
| Power LED | 14 |
| Power Button | 18 |
| Manual Run LED | 2 |
| Manual Run Button | 13 |
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