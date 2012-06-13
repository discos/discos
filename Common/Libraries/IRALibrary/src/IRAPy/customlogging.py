"""
This module is intended to replace the ACS Logging module in 
our python code. 
Basically just substitute the getLogger call to the one contained in this module 
to make logging effective for our custom syntax.
"""
from Acspy.Common import Log
import logging

CUSTOM_ENV = dict(source="custom")
CUSTOM_DATA = dict(data=CUSTOM_ENV)
CUSTOM_EXTRA = dict(extra=CUSTOM_DATA)

class CustomLogger(Log.Logger):
    def __init__(self, name):
        Log.Logger.__init__(self, name)

    def __formatMessage(self, msg):
        return self._Logger__formatMessage(msg)

    def log(self, *args, **kwargs):
        if 'extra' in kwargs:
            if 'data' in kwargs['extra']:
                kwargs['extra']['data'].update(CUSTOM_ENV)
            else:
                kwargs['extra'].update(CUSTOM_DATA)
        else:
            kwargs.update(CUSTOM_EXTRA)
        Log.Logger.log(self, *args, **kwargs)

logging.setLoggerClass(CustomLogger)
logging.root.setLevel(logging.NOTSET)

def getLogger(name=None):
    return logging.getLogger(str(name))

if __name__ == '__main__':
    logger = getLogger("Custom Logger")
    logger.logDebug("messaggio di prova")
