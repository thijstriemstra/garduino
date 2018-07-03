# -*- coding: utf-8 -*-

print("""
 ____  __.  _____    _________
|    |/ _| /  _  \  /   _____/
|      <  /  /_\  \ \_____  \ 
|    |  \/    |    \/        \
|____|__ \____|__  /_______  /
"""
)

def setup_network(ssid, password):
    import network

    sta_if = network.WLAN(network.STA_IF)
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


from pygarden.util import get_config
cfg = get_config('settings.conf')
del get_config

setup_network(
    ssid=cfg.get('network', 'ssid'),
    password=cfg.get('network', 'password')
)
setup_rtc(
    scl_pin=int(cfg.get('rtc', 'scl_pin')),
    sda_pin=int(cfg.get('rtc', 'sda_pin'))
)
del setup_network
del setup_rtc
