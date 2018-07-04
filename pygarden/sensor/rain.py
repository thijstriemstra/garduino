import time

from machine import Pin, ADC


__all__ = ['RainSensor']


MAX_VALUE = 3171


class RainSensor(object):
    """
    Rain sensor is used to detect water.

    See:

    - https://techtutorialsx.com/2016/12/23/esp8266-detecting-rain-drops/
    - http://randomnerdtutorials.com/guide-for-rain-sensor-fc-37-or-yl-83-with-arduino/
    """
    prev = 0

    def __init__(self, label='1', pin_nr=35,
                 topic='devices/{}/rain/{}/percentage', client_id=None):
        self.label = label
        self.pin_nr = pin_nr
        self.topic = topic.format(client_id, self.label)

        self.pin = ADC(Pin(pin_nr))
        self.pin.atten(self.pin.ATTN_11DB)  # Full Scale: 3.3v
        self.pin.width(self.pin.WIDTH_12BIT)  # Set the 12-bit data width

        print("Rain sensor {} is using analog pin {} and topic '{}'".format(
            self.label, self.pin_nr, self.topic))

    def read(self):
        return self.pin.read()

    def publish(self, client):
        sensorValue = self.read()
        msg = str(100 - (sensorValue / (MAX_VALUE / 100)))

        print("* Rain: {}% on topic '{}'".format(msg, self.topic))

        client.publish(self.topic, msg)

    def start(self):
        while True:
            # read the input
            sensorValue = self.read()
            if sensorValue != self.prev:
                perc = str(100 - (sensorValue / (MAX_VALUE / 100)))
                print('{} ({}%)'.format(sensorValue, perc))
                self.prev = sensorValue
                time.sleep(0.1)

    def destroy(self):
        self.pin.deinit()
