#!/bin/bash
set -e

source tools/shared.sh

if [ $# -eq 0 ]
  then
    echo "No filename supplied!"
    exit 1
fi

echo "Uploading: "$1

ampy --baud=$BAUD --port=$PORT --delay=$DELAY put $1

echo
echo $1": OK " $(expr `date +%s` - $start_time)s
