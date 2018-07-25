from pygarden.lib import logging


ASYNC = 'async'
LOBO_C = 'lobo_c'


logger = logging.getLogger(__name__)


class MQTTClient(object):
    """
    MQTT client that publishes messages.
    """
    def __init__(self, client_id, server, connected_cb, disconnected_cb,
                 published_cb, user=None, password=None, ctype=LOBO_C,
                 ssid=None, wifi_pw=None):
        self.client_id = client_id
        self.server = server
        self.user = user
        self.password = password
        self.ctype = ctype
        self.connected_cb = connected_cb
        self.disconnected_cb = disconnected_cb
        self.published_cb = published_cb
        self.ssid = ssid
        self.wifi_pw = wifi_pw

        logger.info('Connection type: {}'.format(self.ctype))

        if self.ctype == ASYNC:
            # Set up client
            mqtt_as.MQTTClient.DEBUG = True
            self.client = mqtt_as.MQTTClient({
                'client_id': self.client_id,
                'server': self.server,
                'user': self.user,
                'password': self.password,
                'port': 0,
                'keepalive': 120,
                'ping_interval': 0,
                'ssl': False,
                'ssl_params': {},
                'response_time': 10,
                'clean_init': True,
                'clean': True,
                'max_repubs': 4,
                'will': None,
                'subs_cb': lambda *_: None,
                'wifi_coro': lambda *_: None,
                'connect_coro': self.conn_han,
                'ssid': self.ssid,
                'wifi_pw': self.wifi_pw
            })

        elif self.ctype == LOBO_C:
            from network import mqtt
            self.client = mqtt(self.client_id, 'mqtt://' + self.server,
                user=self.user, password=self.password, cleansession=True,
                autoreconnect=True,
                connected_cb=self.connected,
                disconnected_cb=self.disconnected,
                published_cb=self.published
        )

        else:
            from umqtt.robust import MQTTClient
            self.client = MQTTClient(
                client_id=self.client_id,
                server=self.server,
                user=self.user,
                password=self.password
            )

    def connect(self):
        if self.ctype == ASYNC:
            try:
                await self.client.connect()
            except OSError:
                logger.error('Connection failed.')
                return

        elif self.ctype == LOBO_C:
            self.client.start()

        else:
            self.client.connect()

    def disconnect(self):
        if self.ctype == LOBO_C:
            self.client.stop()
        else:
            self.client.disconnect()

    async def conn_han(self, client):
        # await client.subscribe('foo_topic', 1)
        self.connected_cb(client)

    def connected(self, task):
        logger.info("[{}] Connected".format(task))

        self.connected_cb(task)

    def disconnected(self, task):
        logger.info("[{}] Disconnected".format(task))

        self.disconnected_cb(task)

    def published(self, pub):
        logger.debug("[{}] Published: {}".format(pub[0], pub[1]))

        self.published_cb(pub)
