from machine import deepsleep
from network import STA_IF, WLAN

from pygarden.mqtt import MQTTClient


__all__ = ['run', 'create_sensors']


class Application(object):
    def __init__(self, client_id, server, sensors, interval=60,
                 user=None, password=None):
        """
        :param client_id: Unique MQTT client ID
        :type client_id: str
        :param server: MQTT broker IP/hostname
        :type server: str
        :param sensors: List of sensors objects.
        :type sensors: list
        :param interval: How often to publish data
        :type interval: int
        """
        self.client_id = client_id
        self.server = server
        self.sensors = sensors
        self.user = user
        self.password = password
        self.interval = interval

        print('Application ready - using {} sensors'.format(
            len(self.sensors)))
        if len(sensors) > 0:
            print()
            for sensor in self.sensors:
                print(' - {}'.format(sensor))
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
            connected_cb=self.connected
        )
        self.client.connect()

    def connected(self, task):
        print('Connection: OK')
        print('-' * 40)
        print()

        # publish
        self.publish()

        # close connection
        self.client.disconnect()

        # go to sleep
        self.sleep()

    def sleep(self):
        print('Going to sleep for {} seconds...'.format(self.interval))
        print('*' * 40)
        print()

        deepsleep(self.interval * 1000)

    def stop(self):
        """
        Destroy sensors.
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
            sensor.publish(self.client.client)

        print()
        print('*' * 20)
        print('Published data: OK')
        print('*' * 20)
        print()


def isEnabled(cfg, section):
    return cfg.has_section(section) and (
        str(cfg.get(section, 'enabled')).lower() != 'false')


def create_sensors(cfg):
    """
    """
    sensors = []
    device_id = cfg.get('broker', 'device_id')

    # temperature
    if isEnabled(cfg, 'temperature'):
        from pygarden.sensor.temperature import TemperatureSensor

        temp_sensor = TemperatureSensor(
            pin_nr=int(cfg.get('temperature', 'pin')),
            client_id=device_id
        )
        sensors.append(temp_sensor)

    # light
    if isEnabled(cfg, 'light'):
        from pygarden.sensor.light import LightSensor

        light_sensor = LightSensor(
            i2c_id=int(cfg.get('light', 'i2c_id')),
            sda_pin=int(cfg.get('light', 'sda_pin')),
            scl_pin=int(cfg.get('light', 'scl_pin')),
            client_id=device_id
        )
        sensors.append(light_sensor)

    # soil
    if isEnabled(cfg, 'soil'):
        from pygarden.sensor.soil import SoilSensor
        for index, pin_nr in enumerate(cfg.get('soil', 'pin_nrs').split(',')):
            soil_sensor = SoilSensor(
                label=str(index),
                pin_nr=int(pin_nr),
                client_id=device_id
            )
            sensors.append(soil_sensor)

    # rain
    if isEnabled(cfg, 'rain'):
        from pygarden.sensor.rain import RainSensor
        for index, pin_nr in enumerate(cfg.get('rain', 'pin_nrs').split(',')):
            rain_sensor = RainSensor(
                label=str(index),
                pin_nr=int(pin_nr),
                client_id=device_id
            )
            sensors.append(rain_sensor)

    return sensors


def run(interval, user, password, server, device_id, sensors):
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
        sensors=sensors,
        interval=interval,
        user=user,
        password=password
    )
