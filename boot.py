# -*- coding: utf-8 -*-

from pygarden import util
from app import run as Application

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

# setup logging
util.setupLogging(
    logfile=cfg.get('log', 'logfile')
)

# create network connection
util.setup_network(
    ssid=cfg.get('network', 'ssid'),
    password=cfg.get('network', 'password')
)

# setup realtime clock
if cfg.get('rtc', 'enabled').lower() != 'false':
    util.setup_rtc(
        i2c_id=1,
        scl_pin=int(cfg.get('rtc', 'scl_pin')),
        sda_pin=int(cfg.get('rtc', 'sda_pin')),
        timezone=cfg.get('rtc', 'timezone')
    )
else:
    print('Realtime clock disabled.')

print()
print('Loading application...')
print()

# create application
application = Application(
    interval=int(cfg.get('general', 'interval')),
    user=cfg.get('broker', 'user'),
    password=cfg.get('broker', 'password'),
    server=cfg.get('broker', 'server'),
    device_id=cfg.get('broker', 'device_id'),
    cfg=cfg
)

# cleanup
del util, Application

# start app
if cfg.get('general', 'auto_start').lower() != 'false':
    application.start()
