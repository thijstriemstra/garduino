from pygarden.lib import logging


DEFAULT = 'default'


logger = logging.getLogger(__name__)


class MQTTClient(object):
    """
    """
    def __init__(self, client_id, server, connected_cb,
                 user=None, password=None, ctype=DEFAULT):
        self.client_id = client_id
        self.server = server
        self.user = user
        self.password = password
        self.ctype = ctype
        self.connected_cb = connected_cb

        if self.ctype == DEFAULT:
            from network import mqtt
            self.client = mqtt(self.client_id, 'mqtt://' + self.server,
                user=self.user, password=self.password, cleansession=True,
                connected_cb=self.connected, disconnected_cb=self.disconnected,
                published_cb=self.published)

        else:
            from umqtt.robust import MQTTClient
            self.client = MQTTClient(
                client_id=self.client_id,
                server=self.server,
                user=self.user,
                password=self.password
            )

    def connect(self):
        if self.ctype == DEFAULT:
            self.client.start()
        else:
            self.client.connect()

    def disconnect(self):
        if self.ctype == DEFAULT:
            self.client.free()
        else:
            self.client.disconnect()

    def connected(self, task):
        logger.info("[{}] Connected".format(task))

        self.connected_cb(task)

    def disconnected(self, task):
        logger.info("[{}] Disconnected".format(task))

    def published(self, pub):
        logger.debug("[{}] Published: {}".format(pub[0], pub[1]))
