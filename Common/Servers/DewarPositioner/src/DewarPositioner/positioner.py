from __future__ import with_statement
import threading
import time

import Antenna
import ComponentErrorsImpl
import ComponentErrors
import DerotatorErrorsImpl
import DerotatorErrors 

from maciErrType import CannotGetComponentEx
from Acspy.Servants import ContainerServices
from Acspy.Clients.SimpleClient import PySimpleClient
from DewarPositioner.posgenerator import PosGenerator, PosGeneratorError
from IRAPy import logger


class Positioner(object):

    modes = {
            'updating': ('FIXED', 'OPTIMIZED'),
            'rewinding': ('AUTO', 'MANUAL')
    }

    def __init__(self):
        self.lock = threading.Lock()
        self.control = Control()
        self.posgen = PosGenerator()
        self._setDefaultConfiguration()

    def setup(self, device_name, starting_position=0):
        """Configure the positioner.
        
        The argument `device_name` is the name of the component to drive. For 
        instance, if we want to drive the SRT K band derotator::

            setup('RECEIVERS/SRTKBandDerotator')

        An optional `starting_position` can be given. This position will be
        added to every position we command. A setup() performs a call to 
        Positioner.stopUpdating() in order to stop a previous active thread.
        """
        try:
            self.lock.acquire()
            if self.isUpdating():
                self.stopUpdating()
            self.device_name = device_name
            self.control.starting_position = starting_position
            self.client = PySimpleClient()
            self.device = self.client.getComponent(device_name)
            self.device.setup()
            self._clearOffset()
            self.is_configured = True
            self._start(self.posgen.goto, self.control.starting_position)
        except CannotGetComponentEx, ex:
            raise PositionerError("cannot get the %s component: %s" 
                    %(device_name, ex.message))
        except (DerotatorErrors.PositioningErrorEx, DerotatorErrors.CommunicationErrorEx), ex:
            raise PositionerError("cannot set the %s position: %s" 
                    %(device_name, ex.message))
        except Exception, ex:
            raise PositionerError(ex.message)
        finally:
            self.lock.release()

    def isConfigured(self):
        return self.is_configured

    def getDeviceName(self):
        if self.isConfigured():
            return self.device_name
        else:
            raise NotAllowedError('positioner not configured: a setup() is required')

    def isReady(self):
        return self.isConfigured() and self.device.isReady()

    def isSlewing(self):
        return self.isConfigured() and self.device.isSlewing()

    def isTracking(self):
        return self.isConfigured() and self.device.isTracking()

    def startUpdating(self):
        try:
            self.lock.acquire()
            if not self.isConfigured():
                raise NotAllowedError('positioner not configured: a setup() is required')
            elif not self.isConfiguredForUpdating():
                raise NotAllowedError('not configured for updating: a setUpdatingMode() is required')
            elif self.isUpdating():
                raise NotAllowedError('the positionier is already updating: a stopUpdating() is required')

            mode = self.getUpdatingMode()
            if mode == 'FIXED':
                posgen = self.posgen.fixed
            elif mode == 'OPTIMIZED':
                posgen = self.posgen.optimized
            else:
                raise PositionerError('mode %s unknown' %mode)

            self._start(posgen)
        finally:
            self.lock.release()

    def stopUpdating(self):
        """Stop the updating thread"""
        try:
            self.control.stop = True
            self.t.join(timeout=10)
            if self.t.isAlive():
                PositionerError('thread %s is alive' %self.t.getName())
        except AttributeError:
            pass # self.t is None because the system is not yet configured
        finally:
            self.control.stop = False

    def park(self):
        if self.isConfigured():
            self.stopUpdating()
            try:
                self.lock.acquire()
                self._clearOffset()
                self._start(self.posgen.goto, self.control.starting_position)
                self.is_configured = False
                self.client.releaseComponent(self.device_name)
                self._setDefaultConfiguration()
            finally:
                self.lock.release()
        else:
            raise NotAllowedError('positioner not configured: a setup() is required')

    def setUpdatingMode(self, mode):
        self._setMode('updating', mode)

    def setRewindingMode(self, mode):
        self._setMode('rewinding', mode)

    def _setMode(self, mode_type, mode):
        try:
            modes = Positioner.modes[mode_type]
        except KeyError:
            raise PositionerError("mode type %s not in %s" %(mode_type, Positioner.modes.keys()))
        if mode not in modes:
            raise PositionerError('%s mode %s unknown; allowed modes: %s' %(mode_type, mode, modes))
        else:
            setattr(self, mode_type + 'Mode', mode)

    def isConfiguredForUpdating(self):
        """Return True if an updating mode has been selected"""
        return bool(self.updatingMode)

    def isConfiguredForRewinding(self):
        """Return True if a rewinding mode has been selected"""
        return bool(self.rewindingMode)

    def getUpdatingMode(self):
        return self.updatingMode

    def getRewindingMode(self):
        return self.rewindingMode

    def isTerminated(self):
        if not self.t:
            return True
        else:
            return not self.isUpdating() and not self.t.isAlive() 

    def isUpdating(self):
        return self.control.updating

    def updatePosition(self, device, control, posgen, vargs):
        try:
            control.updating = True

            # Verify the component is ready to move
            if not device.isReady():
                logger.logError("%s not ready to move: a setup() is required" 
                        %device._get_name())
                # TODO: error communication
                return
 
            for position in posgen(*vargs):
                if control.stop:
                    break
                target = control.starting_position + control.offset + position
                if device.getMinLimit() < target < device.getMaxLimit():
                    try:
                        device.setPosition(target)
                        time.sleep(0.1) # TODO: from CDB
                    except (DerotatorErrors.PositioningErrorEx, DerotatorErrors.CommunicationErrorEx), ex:
                        raeson = "cannot set the %s position" %device._get_name()
                        logger.logError('%s: %s' %(raeson, ex.message))
                        # TODO: error communication
                        break
                    except Exception, ex:
                        raeson = "unknown exception setting the %s position" %device._get_name()
                        logger.logError('%s: %s' %(raeson, ex.message))
                        # TODO: error communication
                        break
                else:
                    logger.logError("position %.2f out of range (%.2f, %.2f)" 
                            %(target, device.getMinLimit(), device.getMaxLimit()))
                    # TODO: REWINDING!

        except KeyboardInterrupt:
            logger.logInfo('stopping Positioner.updatePosition() due to KeyboardInterrupt')
        except AttributeError, ex:
            logger.logError('positioner.updatePosition(): attribute error')
            logger.logDebug('positioner.updatePosition(): %s' %ex)
        except Exception, ex:
            logger.logError('unexcpected exception in Positioner.updatePosition(): %s' %ex)
        finally:
            control.updating = False
            # logger.logInfo('Positioner.updatePosition() stopped @ %s' %datetime.utcnow())
            # TODO: set a status bit of the DewarPositioner, in order to communicate the error

    def setOffset(self, offset):
        self._setOffset(offset) # set the flag
        # And now update the device position
        if self.isUpdating():
            return
        elif self.isConfigured():
            self.stopUpdating()
            act_position = self.getPosition()
            self._start(self.posgen.goto, act_position)

    def clearOffset(self):
        self.setOffset(0.0)

    def _setOffset(self, offset):
        self.control.offset = offset

    def _clearOffset(self):
        self._setOffset(0.0)

    def getOffset(self):
        return self.control.offset

    def getStartingPosition(self):
        return self.control.starting_position

    def getPosition(self):
        if self.isConfigured():
            return self.device.getActPosition()
        else:
            raise NotAllowedError('positioner not configured: a setup() is required')

    def _start(self, posgen, *vargs):
        """Start a new process that computes and sets the position"""
        if self.isConfigured():
            self.stopUpdating() # Raise a PositionerError if the process stills alive
            args = (
                    self.device, 
                    self.control,
                    posgen, 
                    vargs
            )
            self.t = ContainerServices.ContainerServices().getThread(
                    name=posgen.__name__, 
                    target=self.updatePosition, 
                    args=args
            )
            self.t.start()
        else:
            raise NotAllowedError('positioner not configured: a setup() is required')

    def _setDefaultConfiguration(self):
        self.is_configured = False
        self.t = None
        self.device_name = ''
        self.rewindingMode = ''
        self.updatingMode = ''


class Control(object):
    def __init__(self):
        self.updating = False
        self.stop = False
        self.starting_position = 0.0
        self.offset = 0.0


class PositionerError(Exception):
    pass

class NotAllowedError(Exception):
    pass


