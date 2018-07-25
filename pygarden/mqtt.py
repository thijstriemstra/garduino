from pygarden.lib import logging
from pygarden.lib.mqtt_as import MQTTClient


ASYNC = 'async'
LOBO_C = 'lobo_c'


logger = logging.getLogger(__name__)


class MQTTClient(object):
    """
    """
    def __init__(self, client_id, server, connected_cb, disconnected_cb,
                 published_cb, user=None, password=None, ctype=ASYNC,
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

        if self.ctype == ASYNC:
            # Set up client
            MQTTClient.DEBUG = True
            self.client = MQTTClient({
                'client_id': self.client_id,
                'server': self.server,
                'user': self.user,
                'password': self.password,
                'keepalive': 120,
                'ssl': False,
                'clean': True,
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
    """
    async def main(client):
        
        n = 0
        while True:
            await asyncio.sleep(5)
            print('publish', n)
            # If WiFi is down the following will pause for the duration.
            await client.publish('result', '{} repubs: {} outages: {}'.format(n,
                client.REPUB_COUNT, outages), qos = 1)
            n += 1
    """
    async def connect(self):
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
