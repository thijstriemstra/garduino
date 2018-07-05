# -*- coding: utf-8 -*-

from app import run as Application

print('Loading application...')
print()

# create application
application = Application(
    interval=int(cfg.get('general', 'interval')),
    user=cfg.get('broker', 'user'),
    password=cfg.get('broker', 'password'),
    server=cfg.get('broker', 'server'),
    device_id=cfg.get('broker', 'device_id')
)
del Application

# start app
# application.start()
