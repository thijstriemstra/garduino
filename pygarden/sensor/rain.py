# Copyright (c) 2018 Collab
# See LICENSE for details.

import time

from machine import Pin, ADC

from pygarden.lib import logging


__all__ = ['RainSensor']


logger = logging.getLogger(__name__)

MAX_VALUE = 3171


class RainSensor(object):
    """
    Rain sensor is used to detect water.

    See:

    - https://techtutorialsx.com/2016/12/23/esp8266-detecting-rain-drops/
    - http://randomnerdtutorials.com/guide-for-rain-sensor-fc-37-or-yl-83-with-arduino/
    """
    prev = 0

    def __init__(self, label='1', pin_nr=35, topic='rain/{}/percentage'):
        self.label = label
        self.pin_nr = pin_nr
        self.topic = topic

        self.pin = ADC(Pin(pin_nr))
        self.pin.atten(self.pin.ATTN_11DB)  # Full Scale: 3.3v
        self.pin.width(self.pin.WIDTH_12BIT)  # Set the 12-bit data width

    def read(self):
        return self.pin.read()

    def publish(self, client):
        sensorValue = self.read()
        msg = str(100 - (sensorValue / (MAX_VALUE / 100)))

        logger.debug("* Rain: {}% on topic '{}'".format(msg, self.topic))

        client.publish(self.topic, msg)

    def start(self):
        while True:
            # read the input
            sensorValue = self.read()
            if sensorValue != self.prev:
                perc = str(100 - (sensorValue / (MAX_VALUE / 100)))
                logger.debug('{} ({}%)'.format(sensorValue, perc))
                self.prev = sensorValue
                time.sleep(0.1)

    def destroy(self):
        self.pin.deinit()

    def __repr__(self, *args, **kwargs):
        return 'RainSensor [label={} pin={}]'.format(
            self.label, self.pin_nr)
