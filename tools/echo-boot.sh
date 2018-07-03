source tools/shared.sh

ampy --baud=$BAUD --port=$PORT --delay=$DELAY get boot.py

echo
echo "OK " $(expr `date +%s` - $start_time)s
