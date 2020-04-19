py-garden
=========

Arduino code for garden monitoring using ESP32.

This project was created using a Lolin32 Lite v1.0.0.

![Lolin32 Lite](../doc/lolin32_lite_v1.0.0.jpg?raw=true "Lolin32 Lite")

## Wiring

| Sensor | ESP32 Pin |
| --- | --- |
| BH1750 Light Sensor SDA | 23 |
| BH1750 Light Sensor SCL | 19 |
| Single Channel Relay | 22 |
| Dual DS18B20 Temperature Sensors | 15 |
| YL-83 Rain Sensor | 1 |
| FC-38 Soil Sensor 1 | 2 |
| FC-38 Soil Sensor 2 | 3 |
| SR04 Ultrasonic Sensor TRG | 4 |
| SR04 Ultrasonic Sensor ECH | 5 |

## Installation

Checkout code:

```console
git clone https://github.com/thijstriemstra/py-garden.git
cd py-garden/arduino
```

## Development

Open `pygarden` project in VSCode with PlatformIO IDE extension.
