py-garden
=========

Arduino code for garden monitoring using ESP32.

This project was created using a Lolin32 Lite v1.0.0.

![Lolin32 Lite](../doc/lolin32_lite_v1.0.0.jpg?raw=true "Lolin32 Lite")

## Wiring

| Sensor | ESP32 Pin |
| --- | --- |
| WIFI Status LED | 4 |
| WIFI Reset Button | 13 |
| Manual Run Button | 18 |
| BH1750 Light Sensor SDA | 23 |
| BH1750 Light Sensor SCL | 19 |
| Single Channel Relay | 22 |
| Dual DS18B20 Temperature Sensors | 15 |
| YL-83 Rain Sensor | 34 |
| FC-38 Soil Sensor 1 | 35 |
| FC-38 Soil Sensor 2 | 32 |
| SR04 Ultrasonic Sensor TRG | 33 |
| SR04 Ultrasonic Sensor ECH | 25 |

## Installation

Checkout code:

```console
git clone https://github.com/thijstriemstra/py-garden.git
cd py-garden/arduino
```

## Development

Open `pygarden` project in VSCode with PlatformIO IDE extension.
