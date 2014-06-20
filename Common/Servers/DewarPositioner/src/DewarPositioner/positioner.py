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
from DewarPositioner.posgenerator import PosGenerator, PosGeneratorError
from IRAPy import logger


class Positioner(object):
    lock = threading.Lock()

    modes = {
            'updating': ('FIXED', 'OPTIMIZED'),
            'rewinding': ('AUTO', 'MANUAL')
    }

    def __init__(self):
        self.control = Control()
        self.posgen = PosGenerator()
        self._setDefaultConfiguration()

    def setup(self, site_info, source, device, starting_position=0):
        """Configure the positioner.
        
        The argument `device_name` is the name of the component to drive. For 
        instance, if we want to drive the SRT K band derotator::

            setup('RECEIVERS/SRTKBandDerotator')

        An optional `starting_position` can be given. This position will be
        added to every position we command. A setup() performs a call to 
        Positioner.stopUpdating() in order to stop a previous active thread.
        TODO: describe source and site_info arguments; source could be None and 
              site_info an empty dictionary.
              be None
        """
        try:
            Positioner.lock.acquire()
            if self.isUpdating():
                self.stopUpdating()
            self.control = Control()
            self.site_info = site_info
            self.source = source
            self.device = device
            self.control.starting_position = starting_position
            self.device.setup()
            self._clearOffset()
            self.is_configured = True
            self._start(self.posgen.goto, self.control.starting_position)
        except (DerotatorErrors.PositioningErrorEx, DerotatorErrors.CommunicationErrorEx), ex:
            raise PositionerError("cannot set the position: %s" %ex.message)
        except Exception, ex:
            raise PositionerError(ex.message)
        finally:
            Positioner.lock.release()

    def isConfigured(self):
        return self.is_configured

    def getDeviceName(self):
        if self.isConfigured():
            return self.device._get_name()
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
            Positioner.lock.acquire()
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

            if not self.site_info:
                raise NotAllowedError('no site information available')
            elif not self.source:
                raise NotAllowedError('no source available')
            else:
                self._start(posgen, self.source, self.site_info)
                self.control.must_update = True
        finally:
            Positioner.lock.release()

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
            self.control.must_update = False

    def park(self):
        if self.isConfigured():
            self.stopUpdating()
            try:
                Positioner.lock.acquire()
                self._clearOffset()
                self._start(self.posgen.goto, self.control.starting_position)
                self.is_configured = False
                self._setDefaultConfiguration()
            finally:
                Positioner.lock.release()
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

    def setOffset(self, offset):
        if not self.isConfigured():
            raise NotAllowedError('positioner not configured: a setup() is required')
        else:
            self._setOffset(offset) # set the flag
            if self.isUpdating():
                return
            else:
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

    @staticmethod
    def updatePosition(device, control, posgen, vargs):
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
            control.must_update = False
        except KeyboardInterrupt:
            logger.logInfo('stopping Positioner.updatePosition() due to KeyboardInterrupt')
        except AttributeError, ex:
            logger.logError('Positioner.updatePosition(): attribute error')
            logger.logDebug('Positioner.updatePosition(): %s' %ex)
        except Exception, ex:
            print ex
            logger.logError('unexcpected exception in Positioner.updatePosition(): %s' %ex)
        finally:
            control.updating = False
            # logger.logInfo('Positioner.updatePosition() stopped @ %s' %datetime.utcnow())
            # TODO: set a status bit of the DewarPositioner, in order to communicate the error


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
                    target=Positioner.updatePosition, 
                    args=args
            )
            self.t.start()
        else:
            raise NotAllowedError('positioner not configured: a setup() is required')

    def status(self):
        """Un thread lanciato da DewarPositionerImpl deve leggere questo stato e
        aggiornare il notification channel. Per capire come si aggiorna il NC vedi
        ~/notification_channel"""
        # if self.control.must_update and not self.isUpdating():
            # Errore, perche' dovrebbe aggiornare ma non lo sta facendo...

        # Legge lo status di self.device. Questo e' un decimal che devo convertire
        # in stringa binaria (con ~/dec2bin), formattato con il numero di bit che
        # mi interessa. Lo stato di device e' dato da:
        #   bit 0: power off
        #   bit 1: failure
        #   bit 2: communication error
        #   bit 3: not ready
        #   bit 4: slewing
        #   bit 5: warning
        # Quindi un valore 16 significa che sono in slewing
        # Leggo anche isReady(), isTracking(), isSlewing()
        # Quando ho letto lo stato del device valuto insieme al must_update e poi
        # restituisca una tupla con gli stati (o meglio un dizionario). Fare anche test
        # return (timestamp, tracking, slewing, ..., ) # I bit del notification channel

    def _setDefaultConfiguration(self):
        self.is_configured = False
        self.t = None
        self.rewindingMode = ''
        self.updatingMode = ''
        self.control = Control()


class Control(object):
    def __init__(self):
        self.updating = False
        self.stop = False
        self.starting_position = 0.0
        self.offset = 0.0
        self.must_update = False


class PositionerError(Exception):
    pass

class NotAllowedError(Exception):
    pass


