#!/bin/bash
set -e

source tools/shared.sh

export firmware=esp32-20180511-v1.9.4.bin

esptool.py --chip $CHIP -p $PORT write_flash \
  -z 0x1000 $firmware
