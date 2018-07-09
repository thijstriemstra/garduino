from machine import Onewire, internal_temp

from pygarden.lib import logging


__all__ = ['TemperatureSensor']

logger = logging.getLogger(__name__)


class TemperatureSensorNotFound(Exception):
    pass


class InternalTemperatureSensor(object):
    """
    Represents the internal ESP32 temperature sensor.
    """
    def convert_read(self):
        return internal_temp()[1]

    def rom_code(self):
        return 'internal'


class TemperatureSensor(object):
    """
    Represents a collection of DS18X20 temperature sensors.
    """
    def __init__(self, pin_nr=23, roms={}, topic='temperature/{}/celsius'):
        """
        Finds address of one or more DS18B20 sensors on bus specified by
        ``pin``.

        :param pin_nr: 1-Wire bus pin
        :type pin_nr: int
        :param roms:
        :type roms: dict
        """
        self.pin_nr = pin_nr
        self.roms = roms
        self.topic = topic

        try:
            self.ow = Onewire(self.pin_nr)
            self.ds1 = Onewire.ds18x20(self.ow, 0)
            self.ds2 = Onewire.ds18x20(self.ow, 1)
        except ValueError as e:
            if str(e) == 'No devices registered on Onewire bus':
                raise TemperatureSensorNotFound(
                    'No temperature sensor(s) found on pin {}'.format(
                    self.pin_nr))

        self.addrs = [self.ds1, self.ds2]
        if not self.addrs:
            raise TemperatureSensorNotFound(
                'No temperature sensor found at bus on pin %d' % self.pin_nr)

        # internal sensor
        self.is1 = InternalTemperatureSensor()
        self.addrs.append(self.is1)

        # labels
        c1 = self.ds1.rom_code()
        c2 = self.ds2.rom_code()
        c3 = 'internal'
        self.labels = {
            c1: self.roms[c1],
            c2: self.roms[c2],
            c3: c3
        }

    def read(self):
        """
        Reads temperature from sensors.

        :return: List of temperatures
        :rtype: list
        """
        temps = []
        for ds in self.addrs:
            temp = ds.convert_read()
            addr = ds.rom_code()
            label = self.labels[addr]
            temps.append({
                'address': addr,
                'temperature': temp,
                'label': label
            })

        return temps

    def publish(self, client):
        sensorValue = self.read()
        for sensor in sensorValue:
            label = sensor['label']
            msg = str(sensor['temperature'])
            tpc = self.topic.format(label)
            logger.debug("* Temperature {}: {} on topic '{}'".format(
                label, msg, tpc))
            client.publish(tpc, msg)

    def destroy(self):
        self.ds1.deinit()
        self.ds2.deinit()
        self.ow.deinit()

    def __repr__(self, *args, **kwargs):
        return 'TemperatureSensor [devices={} pin={}]'.format(
            len(self.addrs), self.pin_nr)
