# Copyright (c) 2018 Collab
# See LICENSE for details.

import logging
from machine import Pin

from pygarden.lib import tm1637


logger = logging.getLogger(__name__)


class TM1637Display(object):
    """
    Quad 7-segment LED module.
    """
    def __init__(self, clk_pin, dio_pin, brightness=7):
        """
        :param clk_pin:
        :type clk_pin: int
        :param dio_pin:
        :type dio_pin: int
        :param brightness: Display brightness level.
        :type brightness: int
        """
        self.brightness = brightness
        self.display = tm1637.TM1637(
            clk=Pin(clk_pin),
            dio=Pin(dio_pin)
        )
        self.clear()

    def show(self, msg):
        """
        Display message.

        :type msg: str
        """
        self.display.show(msg)

    def temperature(self, temp, delay=250):
        """
        Display temperature in celcius.

        :param temp: Temperature (in degrees celsius).
        :type temp: int
        :param delay:
        :type delay: int
        """
        if (temp > 99) or (temp < -9):
            # out of screen bounds so scroll
            self.scroll('{:.0f}*C'.format(temp), delay=delay)
        else:
            self.display.temperature(temp)

    def clear(self):
        """
        Clear display.
        """
        self.display.write([0, 0, 0, 0])

    def brightness(self, level):
        """
        Set the display brightness 0-7.

        :type level: int
        """
        self.display.brightness(level)

    def scroll(self, msg, delay=250):
        """
        Scroll a message.

        :param msg: Message to scroll.
        :type msg: str
        :param delay:
        :type delay: int
        """
        self.display.scroll(msg, delay=delay)

    def all_chars(self, delay=250):
        """
        Scroll through all available characters.

        :param delay:
        :type delay: int
        """
        self.scroll(list(tm1637._SEGMENTS), delay=delay)
