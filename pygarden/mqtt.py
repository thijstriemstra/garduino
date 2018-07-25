from pygarden.lib import logging


ASYNC = 'async'
LOBO_C = 'lobo_c'


logger = logging.getLogger(__name__)


class MQTTClient(object):
    """
    """
    def __init__(self, client_id, server, connected_cb, disconnected_cb,
                 published_cb, user=None, password=None, ctype=ASYNC):
        self.client_id = client_id
        self.server = server
        self.user = user
        self.password = password
        self.ctype = ctype
        self.connected_cb = connected_cb
        self.disconnected_cb = disconnected_cb
        self.published_cb = published_cb

        if self.ctype == ASYNC:
            import uasyncio as asyncio
            from pygarden.lib.mqtt_as import MQTTClient

            self.loop = asyncio.get_event_loop()

            # Set up client
            MQTTClient.DEBUG = True
            #client = MQTTClient(config)

            """
            try:
                loop.run_until_complete(main(client))
            finally:  # Prevent LmacRxBlk:1 errors.
                client.close()
            """

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
        if self.ctype == LOBO_C:
            self.client.start()
        else:
            self.client.connect()

    def disconnect(self):
        if self.ctype == LOBO_C:
            self.client.stop()
        else:
            self.client.disconnect()

    def connected(self, task):
        logger.info("[{}] Connected".format(task))

        self.connected_cb(task)

    def disconnected(self, task):
        logger.info("[{}] Disconnected".format(task))

        self.disconnected_cb(task)

    def published(self, pub):
        logger.debug("[{}] Published: {}".format(pub[0], pub[1]))

        self.published_cb(pub)
