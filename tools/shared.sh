# OSX
export PORT=/dev/tty.wchusbserial1420
# Linux
export PORT=/dev/ttyUSB0

export BAUD=115200
export DELAY=0
export CHIP=esp32

echo "-------------------------------------"
echo "Using "$PORT "(baudrate: "$BAUD")"
echo "-------------------------------------"
echo

start_time=`date +%s`
