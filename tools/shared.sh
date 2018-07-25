export PORT=/dev/ttyUSB0
export BAUD=115200
export DELAY=0
export CHIP=esp32

if [[ "$OSTYPE" == "darwin"* ]]; then
    PORT=/dev/tty.wchusbserial1420
    # Fix for macOS users' "Could not enter raw repl"; try 2.0 and lower from there:
    DELAY=0.5
fi

echo 
echo "-------------------------------------------------"
echo "Connecting to" $CHIP "using" $PORT "(baudrate: "$BAUD")"
echo "-------------------------------------------------"
echo

start_time=`date +%s`
