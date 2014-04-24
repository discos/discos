import inspect
from IRAPy import logger


class LazyLogger:
    def __init__(self, clsname):
        self.clsname = clsname

    def __call__(self, level, obj):
        msg = str(obj) # Get a string representation of the object
        current_frame = inspect.currentframe()
        caller_frame = inspect.getouterframes(current_frame, 2)
        caller_name = caller_frame[1][3]
        msg = msg.lower()
        try:
            method = getattr(logger, 'log' + level.capitalize())
            method('%s.%s(): %s' %(self.clsname, caller_name, msg))
        except AttributeError, ex:
            logger.logDebug(ex.message)
            logger.logAlert('%s.log(): level `%s` not allowed.' 
                    %(self.clsname, level))
            logger.logError('%s.%s(): %s' %(self.clsname, caller_name, msg))
    

