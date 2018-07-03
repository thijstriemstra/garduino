export PORT=/dev/tty.wchusbserial1420
export PORT=/dev/ttyUSB0
export BAUD=115200
export DELAY=1
export CHIP=esp32

echo "Using "$PORT "(baudrate: "$BAUD")"
echo

start_time=`date +%s`
