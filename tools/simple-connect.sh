#!/bin/bash
set -e

echo 'Press CTRL-] to exit.'

source tools/shared.sh

python3 -m serial.tools.miniterm --raw $PORT $BAUD
