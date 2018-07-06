#!/bin/bash
set -e

source tools/shared.sh

esptool.py --chip $CHIP -p $PORT erase_flash
