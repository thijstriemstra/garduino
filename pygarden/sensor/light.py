from machine import I2C

from pygarden.lib.bh1750 import BH1750


__all__ = ['LightSensor']


class LightSensorNotFound(Exception):
    pass


class LightSensor(object):
    """
    Represents a light sensor.
    """
    def __init__(self, i2c_id=1, sda_pin=16, scl_pin=17,
                 topic='devices/{}/light/lux', client_id=None):
        self.topic = topic.format(client_id)
        self.i2c = I2C(id=i2c_id, scl=scl_pin, sda=sda_pin)
        self.addrs = self.i2c.scan()

        if not self.addrs:
            raise LightSensorNotFound(
                'No BH1750 found at bus on SDA pin %d and SCL pin %d' % (
                sda_pin, scl_pin))

        self.sensor = BH1750(self.i2c)

        print("Light sensor is using SDA pin {}, SCL pin {} and topic '{}'".format(
            sda_pin, scl_pin, self.topic))

    def read(self, mode=BH1750.ONCE_HIRES_1):
        """
        Reads and returns light luminance in lumens per m2 (lux).
        """
        return self.sensor.luminance(mode)

    def publish(self, client):
        val = str(self.read())
        print("* Light: {} on topic '{}'".format(val, self.topic))
        client.publish(self.topic, val)

    def destroy(self):
        self.sensor.off()
        self.i2c.deinit()
