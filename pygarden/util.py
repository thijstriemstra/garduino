"""
Utilities.
"""

import time
import utime
from machine import I2C, RTC
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
    """
    sta_if = WLAN(STA_IF)
    if not sta_if.isconnected():
        logger.info('Connecting to {} network...'.format(ssid))
        sta_if.active(True)
        sta_if.connect(ssid, password)
        while not sta_if.isconnected():
            pass
    ncfg = sta_if.ifconfig()
    logger.info('Network config')
    logger.info('--------------')
    logger.info('SSID: {}'.format(ssid))
    logger.info('IP: {}'.format(ncfg[0]))
    logger.info('Router: {}'.format(ncfg[2]))
    logger.info('DNS: {}'.format(ncfg[3]))
    logger.info('--------------')
    logger.info('')


def setup_rtc(i2c_id, scl_pin, sda_pin, timezone='Europe/Amsterdam'):
    """
    Pull time from RTC at startup.
    """
    from pygarden.lib.ds3231 import DS3231

    logger.info('#' * 30)
    logger.info('Realtime clock: bus {} with SDA pin {} and SCL pin {}'.format(
        i2c_id, sda_pin, scl_pin))
    logger.info('Timezone: {}'.format(timezone))

    # sync
    pool_url = 'pool.ntp.org'
    logger.info('Syncing date with {}'.format(pool_url))
    rtc = RTC()
    rtc.ntp_sync(pool_url, 3600, timezone)
    # wait for it
    while rtc.synced() is False:
        time.sleep(1)

    # setup
    i2c = I2C(id=i2c_id, scl=scl_pin, sda=sda_pin, mode=I2C.MASTER)
    d = DS3231(i2c)
    d.save_time()

    # cleanup
    i2c.deinit()

    local_time = utime.strftime(time_fmt, utime.localtime())
    logger.info('Local time: {}'.format(local_time))
    logger.info('#' * 30)


def setupLogging(level=logging.DEBUG, logfile=None):
    """
    """
    logging.basicConfig(
        level=level,
        format="%s ",
        filename=logfile
    )
