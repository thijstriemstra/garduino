# -*- coding: utf-8 -*-

import gc
import sys
import logging

from micropython import mem_info

from pygarden import util

from app import run as Application


print('*' * 40)
print('MicroPython: {}'.format('.'.join(str(x) for x in sys.implementation.version)))
print('ESP-IDF: {} ({})'.format(sys.espidf_info()[0], sys.espidf_info()[2]))
print('Python: {} ({})'.format(sys.version, sys.platform))
print('mpycore: {} ({})'.format(sys.mpycore()[0], sys.mpycore()[1]))
print('*' * 40)
print('')

# load configuration file
cfg = util.get_config()

# setup logging
util.setup_logging(
    logfile=cfg.get('log', 'logfile')
)
logger = logging.getLogger(__name__)

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
        timezone=cfg.get('rtc', 'timezone'),
        hardware=cfg.get('rtc', 'hardware').lower() != 'false'
    )
else:
    print('Realtime clock disabled.')

# setup display
display = None
if cfg.get('display', 'enabled').lower() != 'false':
    if cfg.get('display', 'type').lower() == 'tm1637':
        display = util.setup_tm1637(
            clk=int(cfg.get('display', 'clk_pin')),
            dio=int(cfg.get('display', 'dio_pin'))
        )

print()
print('Loading application...')
print()

try:
    # create application
    application = Application(
        interval=int(cfg.get('general', 'interval')),
        user=cfg.get('broker', 'user'),
        password=cfg.get('broker', 'password'),
        server=cfg.get('broker', 'server'),
        device_id=cfg.get('broker', 'device_id'),
        display=display,
        cfg=cfg
    )

    # free memory
    gc.collect()
    mem_info()
    del mem_info, sys, gc, util, Application

    # start app
    if cfg.get('general', 'auto_start').lower() != 'false':
        application.start()
except Exception as e:
    logger.exc(e, str(e))
