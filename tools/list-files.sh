#!/bin/bash
set -e

source tools/shared.sh

ampy --baud=$BAUD --port=$PORT --delay=$DELAY ls -l /flash

echo
echo "OK " $(expr `date +%s` - $start_time)s
