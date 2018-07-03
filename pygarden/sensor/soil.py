import time

from machine import Pin, ADC


__all__ = ['SoilSensor']


MAX_VALUE = 3171


class SoilSensor(object):
    """
    Soil moisture sensor, or hygrometer, is used to determine the humidity of
    soil.

    See:

    - http://docs.dfrobot.com.cn/upycraft/4.1.4%20analogRead.py.html
    - https://diyprojects.io/micropython-project-several-ds18b20-probes-publish-measurements-domoticz/
    - http://randomnerdtutorials.com/guide-for-soil-moisture-sensor-yl-69-or-hl-69-with-the-arduino/
    """
    prev = 0

    def __init__(self, label='1', pin_nr=32, topic='devices/{}/soil/{}/percentage',
                 client_id=None):
        self.label = label
        self.pin_nr = pin_nr
        self.topic = 'devices/{}/soil/'.format(client_id) + '{}/percentage'.format(self.label)

        self.pin = ADC(Pin(pin_nr))
        self.pin.atten(self.pin.ATTN_11DB)  # Full Scale: 3.3v
        self.pin.width(self.pin.WIDTH_12BIT)  # Set the 12-bit data width

        print("Soil sensor {} is using analog pin {} and topic '{}'".format(
            self.label, self.pin_nr, self.topic))

    def read(self):
        return self.pin.read()

    def publish(self, client):
        sensorValue = self.read()
        val = str(sensorValue / (MAX_VALUE / 100))
        print("* Soil {}: {}% on topic '{}'".format(
            self.label, val, self.topic))
        client.publish(self.topic, val)

    def start(self):
        while True:
            # read the input
            sensorValue = self.read()
            if sensorValue != self.prev:
                perc = sensorValue / (MAX_VALUE / 100)
                print('{} ({}%)'.format(sensorValue, perc))
                self.prev = sensorValue
                time.sleep(0.1)

    def destroy(self):
        self.pin.deinit()
