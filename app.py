import gc
import uasyncio as asyncio
gc.collect()

from machine import deepsleep
from network import STA_IF, WLAN
gc.collect()

from pygarden.lib import logging
from pygarden.mqtt import MQTTClient
gc.collect()


__all__ = ['run']

logger = logging.getLogger(__name__)


class Application(object):
    def __init__(self, client_id, server, cfg, interval=60,
                 user=None, password=None):
        """
        :param client_id: Unique MQTT client ID
        :type client_id: str
        :param server: MQTT broker IP/hostname
        :type server: str
        :param cfg:
        :param interval: How often to publish data
        :type interval: int
        """
        self.client_id = client_id
        self.server = server
        self.cfg = cfg
        self.user = user
        self.password = password
        self.interval = interval
        self.main_topic = self.cfg.get('general', 'base_topic') + '/' + self.client_id + '/'

        self.loop = asyncio.get_event_loop()

        # create sensors
        self.sensors = self.create_sensors()
        logger.info('Application ready - using {} sensors'.format(
            len(self.sensors)))
        if len(self.sensors) > 0:
            logger.info('')
            for sensor in self.sensors:
                logger.info(' - {}'.format(sensor))
        logger.info('')

    def start(self):
        """
        Start publishing data and go into deep sleep afterwards.
        """
        logger.info('-' * 40)
        logger.info('Connecting to broker: {}'.format(self.server))

        # connect to MQTT server
        self.client = MQTTClient(
            client_id=self.client_id,
            server=self.server,
            user=self.user,
            password=self.password,
            connected_cb=self.connected,
            disconnected_cb=self.disconnected,
            published_cb=self.published,
            ssid=self.cfg.get('network', 'ssid'),
            wifi_pw=self.cfg.get('network', 'password')
        )
        try:
            self.loop.run_until_complete(self.client.connect())
        finally:
            # prevent LmacRxBlk:1 errors.
            self.client.close()

    def connected(self, task):
        """
        Connected to MQTT broker.
        """
        logger.info('Connection: OK')
        logger.info('-' * 40)
        logger.info('')

        # publish
        self.publish()

        # shutdown
        self.stop()

    def disconnected(self, task):
        """
        Disconnected from MQTT broker.
        """
        logger.info('Disconnected')
        logger.info('-' * 40)
        logger.info('')

    def sleep(self):
        """
        Place device into deepsleep.
        """
        logger.info('Going to sleep for {} seconds...'.format(self.interval))
        logger.info('*' * 40)
        logger.info('')

        deepsleep(self.interval * 1000)

    def stop(self):
        """
        Disconnect from MQTT broker.
        """
        # close connection
        # self.client.disconnect()

        self.destroy()

    def destroy(self):
        """
        Destroy sensors and go to sleep.
        """
        # destroy
        logger.debug('Destroying {} sensors...'.format(len(self.sensors)))
        for sensor in self.sensors:
            sensor.destroy()

        logger.info('Destroy: OK')
        logger.info('')

        # go to sleep
        self.sleep()

    def publish(self):
        """
        Start publishing sensor data.
        """
        logger.info('*' * 20)
        logger.info('Publishing data...')
        logger.info('*' * 20)
        logger.info('')

        # publish
        for sensor in self.sensors:
            sensor.publish(self.client.client)

        logger.info('')
        logger.info('*' * 20)
        logger.info('Published data: OK')
        logger.info('*' * 20)
        logger.info('')

    def published(self, pub):
        """
        Called when a message has been published.
        """
        logger.info('[{}] Published: {}'.format(pub[0], pub[1]))

    def isEnabled(self, section):
        """
        Check if ``section`` is present and enabled in configuration file.

        :param section:
        :type section: str
        """
        return self.cfg.has_section(section) and (
            str(self.cfg.get(section, 'enabled')).lower() != 'false')

    def create_sensors(self):
        """
        """
        sensors = []

        # temperature
        if self.isEnabled('temperature'):
            from pygarden.sensor.temperature import TemperatureSensor

            roms = {}
            for rom in self.cfg.get('temperature', 'roms').split(','):
                roms[rom] = self.cfg.get('temperature', rom)

            temp_sensor = TemperatureSensor(
                pin_nr=int(self.cfg.get('temperature', 'pin')),
                roms=roms,
                topic=self.main_topic + 'temperature_{}'
            )
            sensors.append(temp_sensor)

        # light
        if self.isEnabled('light'):
            from pygarden.sensor.light import LightSensor

            light_sensor = LightSensor(
                i2c_id=int(self.cfg.get('light', 'i2c_id')),
                sda_pin=int(self.cfg.get('light', 'sda_pin')),
                scl_pin=int(self.cfg.get('light', 'scl_pin')),
                topic=self.main_topic + 'light'
            )
            sensors.append(light_sensor)

        # soil
        if self.isEnabled('soil'):
            from pygarden.sensor.soil import SoilSensor
            for index, pin_nr in enumerate(
                    self.cfg.get('soil', 'pin_nrs').split(',')):
                label = str(index)
                soil_sensor = SoilSensor(
                    label=label,
                    pin_nr=int(pin_nr),
                    topic=self.main_topic + 'soil_' + label
                )
                sensors.append(soil_sensor)

        # rain
        if self.isEnabled('rain'):
            from pygarden.sensor.rain import RainSensor
            for index, pin_nr in enumerate(
                    self.cfg.get('rain', 'pin_nrs').split(',')):
                label = str(index)
                rain_sensor = RainSensor(
                    label=label,
                    pin_nr=int(pin_nr),
                    topic=self.main_topic + 'rain_' + label
                )
                sensors.append(rain_sensor)

        return sensors


def run(interval, user, password, server, device_id, cfg):
    """
    Create and return application.
    """
    logger.info('+' * 40)
    logger.info('Device: {}'.format(device_id))
    logger.info('Interval: {} seconds'.format(interval))

    sta_if = WLAN(STA_IF)
    if not sta_if.isconnected():
        logger.info('No internet connection.')
    else:
        ip = sta_if.ifconfig()[0]
        logger.info('Local IP address: {}'.format(ip))

    logger.info('+' * 40)
    logger.info('')

    logger.info('-' * 40)
    logger.info('MQTT broker: {}'.format(server))
    logger.info('Username: {}'.format(user))
    logger.info('-' * 40)
    logger.info('')

    return Application(
        client_id=device_id,
        server=server,
        cfg=cfg,
        interval=interval,
        user=user,
        password=password
    )
