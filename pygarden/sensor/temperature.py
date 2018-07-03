from machine import Onewire, internal_temp


__all__ = ['TemperatureSensor']


class TemperatureSensorNotFound(Exception):
    pass


class InternalTemperatureSensor(object):
    """
    Represents the internal ESP32 temperature sensor.
    """
    def read_temp(self):
        return internal_temp()[1]

    def rom_code(self):
        return 'internal'


class TemperatureSensor(object):
    """
    Represents a collection of DS18X20 temperature sensors.
    """
    def __init__(self, pin_nr=23, topic='devices/{}/temperature/{}/celsius', client_id=None):
        """
        Finds address of one or more DS18B20 sensors on bus specified by ``pin``.

        :param pin: 1-Wire bus pin
        :type pin: int
        """
        self.pin_nr = pin_nr
        self.topic = topic.format(client_id, '{}')

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
                'No DS18B20 found at bus on pin %d' % self.pin_nr)

        # internal sensor
        self.is1 = InternalTemperatureSensor()
        self.addrs.append(self.is1)

        for index, addr in enumerate(self.addrs):
            print("Temperature sensor {} is using pin {} and topic '{}'".format(
                index, self.pin_nr, self.topic.format(addr.rom_code())))

    def read(self):
        """
        Reads temperature from sensors.

        :return: List of temperatures
        :rtype: list
        """
        temps = []
        for ds in self.addrs:
            temp = ds.read_temp()
            addr = ds.rom_code()
            temps.append({
                'address': addr,
                'temperature': temp
            })

        return temps

    def publish(self, client):
        sensorValue = self.read()
        for sensor in sensorValue:
            addr = sensor['address']
            val = str(sensor['temperature'])
            tpc = self.topic.format(addr)
            print("* Temperature {}: {} on topic '{}'".format(
                addr, val, tpc))
            client.publish(tpc, val)

    def destroy(self):
        self.ds1.deinit()
        self.ds2.deinit()
        self.ow.deinit()
