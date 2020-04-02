# Copyright (c) 2018-2020 Collab
# See LICENSE for details.

"""
Utilities.
"""

import time
import utime
from network import WLAN, STA_IF

from pygarden.lib import logging


logger = logging.getLogger(__name__)

time_fmt = '%Y-%m-%d %H:%M:%S'


def get_config(fname='settings.conf'):
    """
    Load configuration file.
    """
    from pygarden.lib.configparser.ConfigParser import ConfigParser

    cfg = ConfigParser()
    cfg.read(fname)

    return cfg


def setup_network(ssid, password):
    """
    Create WIFI network connection.

    :type ssid: str
    :type password: str
    """
    sta_if = WLAN(STA_IF)
    if not sta_if.isconnected():
        logger.info('Connecting to {} network...'.format(ssid))
        sta_if.active(True)
        sta_if.connect(ssid, password)
        while not sta_if.isconnected():
            pass
    ncfg = sta_if.ifconfig()
    logger.info('--------------')
    logger.info('Network')
    logger.info('SSID: {}'.format(ssid))
    logger.info('IP: {}'.format(ncfg[0]))
    logger.info('Router: {}'.format(ncfg[2]))
    logger.info('DNS: {}'.format(ncfg[3]))
    logger.info('--------------')
    logger.info('')


def setup_rtc(i2c_id=0, scl_pin=22, sda_pin=21, timezone='Europe/Amsterdam',
              hardware=True):
    """
    Pull time from RTC at startup.
    """
    from machine import RTC

    logger.info('#' * 30)
    logger.info('Timezone: {}'.format(timezone))

    # sync time from server
    pool_url = 'pool.ntp.org'
    logger.info('Syncing date with {}'.format(pool_url))
    rtc = RTC()
    rtc.ntp_sync(pool_url, 3600, timezone)
    # wait for it
    while rtc.synced() is False:
        time.sleep(1)

    if hardware is True:
        from machine import I2C
        from pygarden.lib.ds3231 import DS3231

        # setup hardware clock
        logger.info('Realtime clock: bus {} with SDA pin {} and SCL pin {}'.format(
            i2c_id, sda_pin, scl_pin))
        i2c = I2C(id=i2c_id, scl=scl_pin, sda=sda_pin, mode=I2C.MASTER)
        d = DS3231(i2c)
        d.save_time()

        # cleanup
        i2c.deinit()

    local_time = utime.strftime(time_fmt, utime.localtime())
    logger.info('Local time: {}'.format(local_time))
    logger.info('#' * 30)


def setup_tm1637(clk_pin, dio_pin):
    """
    :type clk_pin: int
    :type dio_pin: int
    """
    from pygarden.display import TM1637Display

    return TM1637Display(clk_pin=clk_pin, dio_pin=dio_pin)


def setup_switch(left_pin=34, right_pin=35):
    """
    """
    from pygarden.switch import ThreeWaySwitch

    return ThreeWaySwitch(left_pin=left_pin, right_pin=right_pin)


def setup_logging(level=logging.DEBUG, logfile=None):
    """
    Setup logging.
    """
    logging.basicConfig(
        level=level,
        format="%s ",
        filename=logfile
    )
