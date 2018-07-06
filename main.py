# -*- coding: utf-8 -*-

from app import run as Application

print()
print('Loading application...')
print()

# create application
application = Application(
    interval=int(cfg.get('general', 'interval')),
    user=cfg.get('broker', 'user'),
    password=cfg.get('broker', 'password'),
    server=cfg.get('broker', 'server'),
    device_id=cfg.get('broker', 'device_id'),
    cfg=cfg
)
del Application

# start app
if cfg.get('general', 'auto_start').lower() != 'false':
    application.start()
