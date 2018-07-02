source tools/shared.sh

esptool.py --chip esp32 -p $PORT erase_flash
