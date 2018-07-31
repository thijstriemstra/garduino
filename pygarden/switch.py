from machine import Pin


class ThreeWaySwitch(object):
    """
    """
    def __init__(self, left_pin=34, right_pin=35):
        self.left_pin = left_pin
        self.right_pin = right_pin

        self.left = Pin(self.left_pin, handler=self.left_handler)
        self.right = Pin(self.right_pin, handler=self.right_handler)

    def left_handler(self):
        logger.debug('Pressed left!')

        self.read()

    def right_handler(self):
        logger.debug('Pressed right!')

        self.read()

    def read(self):
        left_val = self.left.value()
        right_val = self.right.value()

        logger.debug('left: {} - right: {}'.format(
            left_val, right_val))
