from __future__ import with_statement
import threading
import datetime
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
    general_lock = threading.Lock()
    rewinding_lock = threading.Lock()

    modes = {
            'updating': ('FIXED', 'OPTIMIZED'),
            'rewinding': ('AUTO', 'MANUAL')
    }

    def __init__(self, cdb_info):
        self.rewinding_timeout = cdb_info['rewinding_timeout']
        self.rewinding_sleep_time = cdb_info['rewinding_sleep_time']
        self.updating_time = cdb_info['updating_time']
        self.control = Control()
        self.posgen = PosGenerator()
        self._setDefaultConfiguration()

    def setup(self, site_info, source, device, starting_position=0):
        """Configure the positioner.
        
        A setup() performs a call to Positioner.stopUpdating() in order to 
        stop a previous active thread.
        """
        try:
            Positioner.general_lock.acquire()
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
            Positioner.general_lock.release()

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
        return self.isConfigured() and \
               self.device.isTracking() and not \
               self.isRewinding() and not \
               self.isRewindingRequired()

    def setPosition(self, position):
        target = (
                self.control.starting_position +
                self.control.offset +
                self.control.rewinding_offset +
                position
        )
        if self.device.getMinLimit() < target < self.device.getMaxLimit():
            try:
                self.device.setPosition(target)
            except (DerotatorErrors.PositioningErrorEx, DerotatorErrors.CommunicationErrorEx), ex:
                raeson = "cannot set the %s position" %self.device._get_name()
                logger.logError('%s: %s' %(raeson, ex.message))
                raise PositionerError(raeson)
            except Exception, ex:
                raeson = "unknown exception setting the %s position" %self.device._get_name()
                logger.logError('%s: %s' %(raeson, ex.message))
                raise PositionerError(raeson)
        else:
            raise OutOfRangeError("position %.2f out of range {%.2f, %.2f}" 
                    %(target, self.device.getMinLimit(), self.device.getMaxLimit()))

    def startUpdating(self):
        try:
            Positioner.general_lock.acquire()
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
            Positioner.general_lock.release()

    def updatePosition(self, posgen, vargs):
        try:
            self.control.is_rewinding_required = False
            self.control.is_updating = True
            self.control.rewinding_offset = 0

            if not self.device.isReady():
                logger.logError("%s not ready to move: a setup() is required" 
                        %self.device._get_name())
                return
 
            for position in posgen(*vargs):
                if self.control.stop:
                    break
                else:
                    try:
                        self.setPosition(position)
                        time.sleep(self.updating_time)
                    except OutOfRangeError, ex:
                        logger.logInfo(ex.message)
                        if self.control.modes['rewinding'] == 'AUTO':
                            self.rewind(number_of_feeds=None) # Blocking call
                        else:
                            if self.control.modes['rewinding'] == 'MANUAL':
                                self.control.is_rewinding_required = True
                            else:
                                logger.logError('wrong rewinding mode: %s' %self.control.modes['rewinding'])
                            break
                    except Exception, ex:
                        logger.logError(ex.message)
                        break
            self.control.must_update = False
        except KeyboardInterrupt:
            logger.logInfo('stopping Positioner.updatePosition() due to KeyboardInterrupt')
        except AttributeError, ex:
            logger.logError('Positioner.updatePosition(): attribute error')
            logger.logDebug('Positioner.updatePosition(): %s' %ex)
        except PositionerError, ex:
            logger.logError('Positioner.updatePosition(): %s' %ex.message)
        except Exception, ex:
            logger.logError('unexcpected exception in Positioner.updatePosition(): %s' %ex)
        finally:
            self.control.is_updating = False

    def rewind(self, number_of_feeds=None):
        if not self.isConfigured():
            raise NotAllowedError('positioner not configured: a setup() is required')

        try:
            Positioner.rewinding_lock.acquire()
            logger.logInfo('starting the rewinding...')
            self.control.is_rewinding = True
            act_pos, space = self.getRewindingParameters(number_of_feeds)
            self.control.rewinding_offset += space
            self.setPosition(act_pos)
            starting_time = now = datetime.datetime.now()
            while (now - starting_time).seconds < self.rewinding_timeout:
                if self.device.isTracking():
                    break
                else:
                    time.sleep(self.rewinding_sleep_time)
                    now = datetime.datetime.now()
            else:
                raise PositionerError('%ss exceeded' %self.rewinding_timeout)

            self.control.is_rewinding_required = False
        except Exception, ex:
            self.control.is_rewinding_required = True
            raise PositionerError(ex.message)
        finally:
            self.control.is_rewinding = False
            self.control.rewinding_offset = 0
            Positioner.rewinding_lock.release()

    def getRewindingParameters(self, number_of_feeds):
        if not self.isConfigured():
            raise NotAllowedError('positioner not configured: a setup() is required')

        if number_of_feeds != None and number_of_feeds <= 0:
            raise PositionerError('the number of feeds must be positive')

        try:
            act_pos = self.device.getActPosition()
        except Exception, ex:
            raeson = 'cannot get the device position: %s' %ex.message
            raise PositionerError(raeson)

        lspace = act_pos - self.device.getMinLimit() # Space on the left
        rspace = self.device.getMaxLimit() - act_pos # Space on the right
        sign = -1 if lspace >= rspace else 1
        space = max(lspace, rspace)
        max_number_of_feeds = int(space // self.device.getStep())
        if number_of_feeds == None:
            n = max_number_of_feeds
        elif number_of_feeds <= max_number_of_feeds:
            n = number_of_feeds
        else:
            raise PositionerError('actual pos: {%.1f} -> max number of feeds: {%d} (%s given)'
                    %(act_pos, max_number_of_feeds, number_of_feeds))

        rewinding_value = sign * n * self.device.getStep()
        return (act_pos, rewinding_value)

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
                Positioner.general_lock.acquire()
                self._clearOffset()
                self._start(self.posgen.goto, self.control.starting_position)
                self.is_configured = False
                self._setDefaultConfiguration()
            finally:
                Positioner.general_lock.release()
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
            self.control.modes[mode_type] = mode

    def isConfiguredForUpdating(self):
        """Return True if an updating mode has been selected"""
        return bool(self.control.modes['updating'])

    def isConfiguredForRewinding(self):
        """Return True if a rewinding mode has been selected"""
        return bool(self.control.modes['rewinding'])

    def getUpdatingMode(self):
        return self.control.modes['updating']

    def getRewindingMode(self):
        return self.control.modes['rewinding']

    def mustUpdate(self):
        return self.control.must_update

    def isTerminated(self):
        if not self.t:
            return True
        else:
            return not self.isUpdating() and not self.t.isAlive() 

    def isUpdating(self):
        return self.control.is_updating

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

    def getRewindingOffset(self):
        return self.control.rewinding_offset

    def isRewindingRequired(self):
        return self.control.is_rewinding_required and not self.control.is_rewinding

    def isRewinding(self):
        return self.control.is_rewinding

    def getPosition(self):
        if self.isConfigured():
            return self.device.getActPosition()
        else:
            raise NotAllowedError('positioner not configured: a setup() is required')

    def _start(self, posgen, *vargs):
        """Start a new process that computes and sets the position"""
        if self.isConfigured():
            self.stopUpdating() # Raise a PositionerError if the process stills alive
            self.t = ContainerServices.ContainerServices().getThread(
                    name=posgen.__name__, 
                    target=self.updatePosition, 
                    args=(posgen, vargs)
            )
            self.t.start()
        else:
            raise NotAllowedError('positioner not configured: a setup() is required')

    def getStatus(self):
        """Read the status of the device and return a string.

        The returned string represents a bit pattern with the following meaning:

            bit 0: ready
            bit 1: tracking
            bit 2: updating
            bit 3: slewing
            bit 4: warning
            bit 5: failure

        That means the string '000001' means the positioner is ready, the string
        '100000' means the positioner is in failure, the string '001011' 
        indicates the positioner is ready, it is slewing and tracking, and so on.
        This method does not raise any exception, but always returns a status code.
        """
        try:
            Positioner.general_lock.acquire()
            if not self.isConfigured():
                raise NotAllowedError('positioner not configured: a setup() is required')

            failure = False
            warning = False
            if self.mustUpdate() and not self.isUpdating():
                failure = True

            try:
                status_obj = self.device._get_status()
            except Exception, ex:
                raise PositionerError('cannot get the device status property: %s' %ex.message)

            try:
                device_status, compl = status_obj.get_sync()
            except Exception, ex:
                raise PositionerError('cannot get the device status value: %s' %ex.message)

            if compl.code:
                raise PositionerError('the device status value is not valid')
            else:
                # The generic derotator' status is a decimal number. When converted
                # to binary, its meaning is the following:
                #
                #     bit 0: power off
                #     bit 1: failure
                #     bit 2: communication error
                #     bit 3: not ready
                #     bit 4: slewing
                #     bit 5: warning
                #
                # A decimal value of 16 corresponds to the binary value 10000. We must
                # read the bits from right to left, so the bit with value 1 is the
                # bit 4, and that means the device is slewing.
                # We want a string of 6 bits. For instance, for a decimal value of
                # 16 we want to get the string 010000 instead of 10000
                try:
                    binrepr = Status.dec2bin(device_status, 6) # A string of 6 values 
                except Exception, ex:
                    raise PositionerError('error in Status.dec2bin(): %s' %ex.message)

                po, f, ce, nr, s, w = [bool(int(item)) for item in reversed(binrepr)]
                if po:
                    logger.logError('the device is power off')
                    failure = True
                elif f:
                    logger.logError('the device has a failure')
                    failure = True
                elif ce:
                    logger.logError('cannot comunicate with the device')
                    failure = True

            status = ''
            status += '1' if failure else '0'
            status += '1' if warning else '0'
            status += '1' if self.isSlewing() else '0'
            status += '1' if self.isUpdating() else '0'
            status += '1' if self.isTracking() else '0'
            status += '1' if self.isReady() else '0'
            return status
        except (NotAllowedError), ex:
            return '000000' # Not ready
        except Exception, ex:
            logger.logError(ex.message)
            return '100000' # Failure
        finally:
            Positioner.general_lock.release()

    def _setDefaultConfiguration(self):
        self.is_configured = False
        self.t = None
        self.control = Control()


class Control(object):
    def __init__(self):
        self.is_updating = False
        self.must_update = False
        self.stop = False
        self.starting_position = 0.0
        self.offset = 0.0
        self.rewinding_offset = 0.0
        self.is_rewinding_required = False
        self.is_rewinding = False
        self.modes = {'rewinding': '', 'updating': ''}


class Status(object):
    @staticmethod
    def dec2bin(n, nbits=None):
        """Convert a decimal number to its binary value as a string.

        The paramter `n` represents the decimal value and `nbits` the number of
        bit of the resulting string. Some examples::

            >>> Status.dec2bin(10)
            '01010'
            >>> Status.dec2bin(10, 6)
            '001010'
            >>> Status.dec2bin(16, 6)
            '010000'
            >>> Status.dec2bin(16, 5)
            '010000'
            >>> Status.dec2bin(16, 10)
            '0000010000'
            >>> Status.dec2bin(0, 6)
            '000000'
        """
        if n == 0:
            value = '0'
        else:
            value = Status.dec2bin(n//2) + str(n%2)

        if nbits:
            return value if len(value) >= nbits else '0'*(nbits - len(value)) + value
        else:
            return value


class PositionerError(Exception):
    pass


class NotAllowedError(Exception):
    pass


class OutOfRangeError(Exception):
    pass


if __name__ == '__main__':
    import doctest
    doctest.testmod()
