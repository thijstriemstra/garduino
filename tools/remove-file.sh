source tools/shared.sh

if [ $# -eq 0 ]
  then
    echo "No filename supplied!"
    exit 1
fi

ampy --baud=$BAUD --port=$PORT --delay=$DELAY rmdir $1
