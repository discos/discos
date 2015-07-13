"""
This module is intended to replace the ACS Logging module in our python code. 
This is necessary as our logging events contain a key-value pair of extra data used to distinguish them 
from the ACS system log.
"""
from Acspy.Common import Log
import logging

"""
Key-Value pair distinguishing our custom logging.
"""
CUSTOM_ENV = dict(source="custom")
CUSTOM_DATA = dict(data=CUSTOM_ENV)
CUSTOM_EXTRA = dict(extra=CUSTOM_DATA)

class CustomLogger(Log.Logger):
    """This class replaces the standard logging functionalities by overloading the log method. 
    Each log event now will have additional custom key-value data pairs.
    """
    def __init__(self, name):
        Log.Logger.__init__(self, name)

    def __formatMessage(self, msg):
        return self._Logger__formatMessage(msg)

    def log(self, level, msg, *args, **kwargs):
        """Add key-value custom data to the LogRecord structure
        """
        if 'extra' in kwargs:
            if 'data' in kwargs['extra']:
                kwargs['extra']['data'].update(CUSTOM_ENV)
            else:
                kwargs['extra'].update(CUSTOM_DATA)
        else:
            kwargs.update(CUSTOM_EXTRA)
        msg = msg.replace('[', '{')
        msg = msg.replace(']', '}')
        Log.Logger.log(self, level, msg, *args, **kwargs)

logging.setLoggerClass(CustomLogger)
logging.root.setLevel(logging.NOTSET)

def getLogger(name=None):
    """Get our custom logger form the system
    """
    return logging.getLogger(str(name))

if __name__ == '__main__':
    logger = getLogger("Custom Logger")
    logger.logDebug("Custom DEBUG message")
    logger.logWarning("Custom WARNING message")
    logger.logInfo("Custom INFO message")
    logger.logAlert("Custom ALERT message")
    logger.logCritical("Custom CRITICAL message")
