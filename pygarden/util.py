"""
Utilities.
"""


def get_config(fname):
    from pygarden.configparser.ConfigParser import ConfigParser

    cfg = ConfigParser()
    cfg.read(fname)

    return cfg
