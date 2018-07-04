from machine import deepsleep
from network import STA_IF, WLAN

from pygarden.sensor.soil import SoilSensor
from pygarden.sensor.rain import RainSensor
from pygarden.sensor.light import LightSensor
from pygarden.sensor.temperature import TemperatureSensor

from umqtt.robust import MQTTClient


__all__ = ['Application', 'run']


class Application(object):
    def __init__(self, client_id, server, interval=60,
                 user=None, password=None):
        """
        :param client_id: Unique MQTT client ID
        :type client_id: str
        :param server: MQTT broker IP/hostname
        :type server: str
        :param interval: How often to publish data
        :type interval: int
        """
        self.client_id = client_id
        self.server = server
        self.user = user
        self.password = password
        self.interval = interval

        # setup sensors
        print('=' * 80)
        print()
        self.soil_sensor1 = SoilSensor(
            label='1',
            pin_nr=32,
            client_id=self.client_id
        )
        self.soil_sensor2 = SoilSensor(
            label='2',
            pin_nr=33,
            client_id=self.client_id
        )
        self.rain_sensor = RainSensor(
            label='1',
            pin_nr=35,
            client_id=self.client_id
        )
        self.temp_sensor = TemperatureSensor(
            pin_nr=23,
            client_id=self.client_id
        )
        self.light_sensor = LightSensor(
            i2c_id=1,
            sda_pin=16,
            scl_pin=17,
            client_id=self.client_id
        )
        print()
        print('=' * 80)

        self.sensors = [self.soil_sensor1, self.soil_sensor2,
            self.rain_sensor, self.light_sensor, self.temp_sensor]

        print()
        print('Application ready: using {} sensors.'.format(
            len(self.sensors)))
        print()

    def start(self):
        """
        Start publishing data and go into deep sleep afterwards.
        """
        print('-' * 40)
        print('Connecting to broker: {}'.format(self.server))

        # connect to MQTT server
        self.client = MQTTClient(
            client_id=self.client_id,
            server=self.server,
            user=self.user,
            password=self.password,
        )
        self.client.connect()

        print('Connection: OK')
        print('-' * 40)
        print()

        # publish
        self.publish()

        # close connection
        self.client.disconnect()

        # go into deep sleep for a while
        self.sleep()

    def sleep(self):
        print('Going to sleep for {} seconds...'.format(self.interval))
        print('*' * 40)
        print()

        deepsleep(self.interval * 1000)

    def stop(self):
        """
        Stop publishing and destroy sensors.
        """
        print('Destroying {} sensors...'.format(len(self.sensors)))

        for sensor in self.sensors:
            sensor.destroy()

        print('Destroy: OK')
        print()

    def publish(self):
        """
        Start publishing sensor data.
        """
        print('*' * 20)
        print('Publishing data...')
        print('*' * 20)
        print()

        for sensor in self.sensors:
            sensor.publish(self.client)

        print()
        print('*' * 20)
        print('Published data: OK')
        print('*' * 20)
        print()


def run(interval, user, password, server, device_id):
    """
    Create and return application.
    """
    print('+' * 40)
    print('Device: {}'.format(device_id))
    print('Interval: {} seconds'.format(interval))

    sta_if = WLAN(STA_IF)
    if not sta_if.isconnected():
        print('No internet connection.')
    else:
        ip = sta_if.ifconfig()[0]
        print('Local IP address: {}'.format(ip))

    print('+' * 40)
    print()

    print('-' * 40)
    print('MQTT broker: {}'.format(server))
    print('Username: {}'.format(user))
    print('-' * 40)
    print()

    return Application(
        client_id=device_id,
        server=server,
        interval=interval,
        user=user,
        password=password
    )
