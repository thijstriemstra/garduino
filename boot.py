# -*- coding: utf-8 -*-

from pygarden import util

print("""
 ____  __.  _____    _________
|    |/ _| /  _  \  /   _____/
|      <  /  /_\  \ \_____  \ 
|    |  \/    |    \/        \
|____|__ \____|__  /_______  /
"""
)

# load configuration file
cfg = util.get_config()

# create network connection
util.setup_network(
    ssid=cfg.get('network', 'ssid'),
    password=cfg.get('network', 'password')
)

# setup realtime clock
util.setup_rtc(
    scl_pin=int(cfg.get('rtc', 'scl_pin')),
    sda_pin=int(cfg.get('rtc', 'sda_pin'))
)

# cleanup
del util
