"""This method defines a Positioner class...

Usage::

    >>> from trackers import optimized
    >>> sleep_time = 0.1 # 100ms
    >>> p = Positioner()
    >>> p.start(optimized, derotator, mount, sleep_time, site_info) # Avvia il processo
    >>> p.terminate() # Termina il processo
    >>> p.start() # Riavvia il processo...

If you start a process
"""
import time
from datetime import datetime
from multiprocessing import Process, Value
from IRAPy import logger

class PositionerError(Exception):
    pass

class Positioner(object):

    def start(self, func_updater, *vargs):
        """Start a new process that computes and sets the position"""
        self.terminate()
        self.args = (func_updater, vargs)
        self.p = Process(target=self.updatePosition, args=self.args)
        self.p.daemon = True
        self.p.start()

    def updatePosition(self, func_updater, func_args):
        try:
            func_updater(*func_args)
        except KeyboardInterrupt:
            logger.logInfo('Stopping Positioner.updatePosition() due to KeyboardInterrupt')
        except AttributeError, ex:
            logger.logError('Positioner.updatePosition(): attribute error')
            logger.logDebug('Positioner.updatePosition(): %s' %ex)
        except Exception, ex:
            logger.logError('Unexcpected exception in Positioner.updatePosition()')
            logger.logDebug('Unexcpected exception in Positioner.updatePosition(): %s' %ex)
        finally:
            # TODO: set a status bit of the DewarPositioner, in order to communicate the error
            logger.logInfo('Positioner.updatePosition() stopped @ %s' %datetime.utcnow())

    def terminate(self):
        """Terminate the process by using the SIGTERM signal"""
        if hasattr(self, 'p'):
            self.p.terminate()
            self.p.join()

    def updating(self):
        if hasattr(self, 'p'):
            return self.p.is_alive()
        else:
            return False

