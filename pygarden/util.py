"""
Utilities.
"""


def get_config(fname='settings.conf'):
    """
    Load configuration file.
    """
    from pygarden.configparser.ConfigParser import ConfigParser

    cfg = ConfigParser()
    cfg.read(fname)

    return cfg


def setup_network(ssid, password):
    """
    Create WIFI network connection.
    """
    from network import WLAN, STA_IF

    sta_if = WLAN(STA_IF)
    if not sta_if.isconnected():
        print('Connecting to {} network...'.format(ssid))
        sta_if.active(True)
        sta_if.connect(ssid, password)
        while not sta_if.isconnected():
            pass
    ncfg = sta_if.ifconfig()
    print('Network config')
    print('--------------')
    print('SSID: {}'.format(ssid))
    print('IP: {}'.format(ncfg[0]))
    print('Router: {}'.format(ncfg[2]))
    print('DNS: {}'.format(ncfg[3]))
    print()


def setup_rtc(scl_pin, sda_pin):
    """
    Pull time from RTC at startup.
    """
    from machine import I2C
    from pygarden.ds3231 import DS3231

    print('#' * 30)
    print('Realtime clock: SDA pin {} and SCL pin {}'.format(
        sda_pin, scl_pin))

    i2c = I2C(id=0, scl=scl_pin, sda=sda_pin)
    d = DS3231(i2c)
    d.get_time(set_rtc=True)
    
    print('Local time: {}'.format('TODO'))
    print('#' * 30)
