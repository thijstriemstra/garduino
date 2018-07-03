"""
Utilities.
"""


def get_config(fname):
    from configparser.ConfigParser import ConfigParser

    cfg = ConfigParser()
    cfg.read(fname)

    return cfg
