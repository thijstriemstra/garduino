source tools/shared.sh

if [ $# -eq 0 ]
  then
    echo "No filename supplied!"
    exit 1
fi

echo "Echo: "$1

ampy --baud=$BAUD --port=$PORT --delay=$DELAY get $1

echo
echo "OK " $(expr `date +%s` - $start_time)s
