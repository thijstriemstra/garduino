# -*- coding: utf-8 -*-

from app import main

__all__ = ['app']


print()
print('Loading application...')
print()

# application
app = main(
    interval=int(cfg.get('general', 'interval')),
    user=cfg.get('broker', 'user'),
    password=cfg.get('broker', 'password'),
    server=cfg.get('broker', 'server'),
    device_id=cfg.get('broker', 'device_id')
)

# start
app.start()
