from machine import I2C

from pygarden.lib import logging
from pygarden.lib.bh1750 import BH1750


__all__ = ['LightSensor']


logger = logging.getLogger(__name__)


class LightSensorNotFound(Exception):
    pass


class LightSensorError(Exception):
    pass


class LightSensor(object):
    """
    Represents a light sensor.
    """
    def __init__(self, i2c_id=0, sda_pin=16, scl_pin=17, topic='light/lux'):
        self.i2c_id = i2c_id
        self.sda_pin = sda_pin
        self.scl_pin = scl_pin
        self.topic = topic

        self.i2c = I2C(id=self.i2c_id, scl=self.scl_pin, sda=self.sda_pin,
                       mode=I2C.MASTER)
        self.addrs = self.i2c.scan()

        if not self.addrs:
            # cleanup
            self.i2c.deinit()

            raise LightSensorNotFound(
                'No light sensor found on bus %d with SDA pin %d and SCL pin %d' % (
                self.i2c_id, sda_pin, scl_pin))

        try:
            self.sensor = BH1750(bus=self.i2c)
            self.sensor.on()
        except OSError as e:
            logger.warning('Addresses found: {}'.format(self.addrs))
            raise LightSensorError(str(e))

    def read(self, mode=BH1750.ONCE_HIRES_1):
        """
        Reads and returns light luminance in lumens per m2 (lux).
        """
        return self.sensor.luminance(mode)

    def publish(self, client):
        msg = str(self.read())
        logger.debug("* Light: {} on topic '{}'".format(msg, self.topic))
        client.publish(self.topic, msg)

    def destroy(self):
        self.sensor.off()
        self.i2c.deinit()

    def __repr__(self, *args, **kwargs):
        return 'LightSensor [bus={} sda={} scl={}]'.format(
            self.i2c_id, self.sda_pin, self.scl_pin)
