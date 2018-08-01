import logging
from machine import Pin

from pygarden.lib import tm1637


logger = logging.getLogger(__name__)


class TM1637Display(object):
    """
    Quad 7-segment LED module.
    """
    def __init__(self, clk_pin, dio_pin):
        self.display = tm1637.TM1637(clk=Pin(clk_pin), dio=Pin(dio_pin))
        self.clear()

    def show(self, msg):
        """
        Display message.
        """
        self.display.show(msg)

    def temperature(self, temp):
        """
        Display temperature in celcius.

        :type temp: int
        """
        self.display.temperature(temp)

    def clear(self):
        """
        Turn all LEDS off.
        """
        self.display.write([0, 0, 0, 0])

    def brightness(self, level):
        """
        Set the display brightness 0-7.
        """
        self.display.brightness(level)

    def scroll(self, msg):
        """
        Scroll a message.
        """
        self.display.scroll(message)

    def all_chars(self):
        """
        Scroll all available characters.
        """
        self.scroll(list(tm1637._SEGMENTS))
