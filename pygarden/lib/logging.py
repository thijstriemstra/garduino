import sys
from utime import strftime, localtime

CRITICAL = 50
ERROR    = 40
WARNING  = 30
INFO     = 20
DEBUG    = 10
NOTSET   = 0

_level_dict = {
    CRITICAL: "CRIT",
    ERROR: "ERROR",
    WARNING: "WARN",
    INFO: "INFO",
    DEBUG: "DEBUG",
}

_stream = sys.stderr


class Logger:

    level = NOTSET

    def __init__(self, name):
        self.name = name

    def _level_str(self, level):
        l = _level_dict.get(level)
        if l is not None:
            return l
        return "LVL%s" % level

    def setLevel(self, level):
        self.level = level

    def isEnabledFor(self, level):
        return level >= (self.level or _level)

    def log(self, level, msg, *args):
        if level >= (self.level or _level):
            default_time_format = '%Y-%m-%d %H:%M:%S'
            now = strftime(default_time_format, localtime())
            timestamp = _format % (now)
            _stream.write(timestamp)
            d = None
            if not args:
                d = msg
            else:
                d = msg % args

            print(d, file=_stream)
            # also write to file
            if _file is not None:
                f = open(_file, 'a')
                # include timestamp and newline
                f.write(timestamp + d + '\n')
                f.close()

    def debug(self, msg, *args):
        self.log(DEBUG, msg, *args)

    def info(self, msg, *args):
        self.log(INFO, msg, *args)

    def warning(self, msg, *args):
        self.log(WARNING, msg, *args)

    def error(self, msg, *args):
        self.log(ERROR, msg, *args)

    def critical(self, msg, *args):
        self.log(CRITICAL, msg, *args)

    def exc(self, e, msg, *args):
        self.log(ERROR, msg, *args)
        sys.print_exception(e, _stream)

    def exception(self, msg, *args):
        self.exc(sys.exc_info()[1], msg, *args)


_level = INFO
_format = "%s "
_file = None
_loggers = {}

def getLogger(name):
    if name in _loggers:
        return _loggers[name]
    l = Logger(name)
    _loggers[name] = l
    return l

def info(msg, *args):
    getLogger(None).info(msg, *args)

def debug(msg, *args):
    getLogger(None).debug(msg, *args)

def basicConfig(level=INFO, format=None, filename=None, stream=None):
    global _level, _stream, _format, _file
    _level = level
    if format:
        _format = format
    if stream:
        _stream = stream
    if filename is not None:
        _file = filename
