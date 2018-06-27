"""
This module is intended to extend the ACS Logging module in our python code.
"""
import logging
import sys
from Acspy.Common import Log
from ACSLog import ACS_LOG_CRITICAL

class CustomLogger(Log.Logger):
    """
    This class replaces the standard logging functionalities by overloading the
    `log` method and by defining a new `logException` method.
    """
    def __init__(self, name):
        self.isUserLogger=False
        Log.Logger.__init__(self, name)

    def __formatMessage(self, msg):
        return self._Logger__formatMessage(msg)

    def log(self, level, msg, *args, **kwargs):
        # Replace some characters in order to avoid parsing errors
        msg = msg.replace('[', '{')
        msg = msg.replace(']', '}')
        Log.Logger.log(self, level, msg, *args, **kwargs)
        
    def logException(self, ex):
        """This method is another workaround to match c++ with buggy python
        behaviour. It results into having a double entry in acs log. The
        LOG_CRITICAL level correspond in the ERROR level...other ACS bug!
        """
        ex.errorTrace = self._fix_error_trace(ex.errorTrace)
        if self.isUserLogger:
            # Get caller name first to log it correctly
            caller = sys._getframe(1).f_code.co_name
            self.log(logging.ERROR, '%s - %s' % (caller, ex.shortDescription))
        ex.log(self, ACS_LOG_CRITICAL)

    def _fix_error_trace(self, trace):
        """In order to avoid XML parsing errors in the CustomLogger component,
        fields errorTrace.file and errorTrace.routine must not have XML key
        characters `<` and `>`, therefore, we replace them with `{` and `}`."""
        trace.file = trace.file.replace('<', '{').replace('>', '}')
        trace.routine = trace.routine.replace('<', '{').replace('>', '}')
        for index in range(len(trace.previousError)):
            previousTrace = trace.previousError[index]
            previousTrace = self._fix_error_trace(previousTrace)
            trace.previousError[index] = previousTrace
        return trace


logging.setLoggerClass(CustomLogger)
logging.root.setLevel(logging.NOTSET)

def getLogger(name=None):
    """Get our custom logger from the system."""
    return logging.getLogger(str(name))

if __name__ == '__main__':
    logger = getLogger("Custom Logger")
    logger.logDebug("Custom DEBUG message")
    logger.logWarning("Custom WARNING message")
    logger.logInfo("Custom INFO message")
    logger.logAlert("Custom ALERT message")
    logger.logCritical("Custom CRITICAL message")
