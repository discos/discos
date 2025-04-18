from __future__ import with_statement
import threading
import datetime
import time

import Management
import Antenna
import DerotatorErrors

from Acspy.Servants import ContainerServices
from Acspy.Common.TimeHelper import getTimeStamp
from DewarPositioner.posgenerator import PosGenerator
from IRAPy import logger

__docformat__ = 'restructuredtext'


class Positioner(object):
    generalLock = threading.Lock()
    rewindingLock = threading.Lock()

    def __init__(self, cdbconf):
        """Initialize the `Positioner` by setting the default values.

        The `cdbconf` parameter is a CDBConf instance.
        """
        self.conf = cdbconf
        self.updatingTime = int(float(self.conf.getAttribute('UpdatingTime')) * 10**7)
        self.trackingLeadTime = int(float(self.conf.getAttribute('TrackingLeadTime')) * 10**7)
        self.posgen = PosGenerator(self.updatingTime, self.trackingLeadTime)
        self._setDefault()


    def setup(self, siteInfo, source, device, setupPosition=0):
        """Configure the positioner.

        A `setup()` performs a call to Positioner.stopUpdating() in order to
        stop a previous active thread.

        :param siteInfo: {'attr_name': value} dictionary of CDB attributes
        :type siteInfo: dict.
        :param source: object with the methods ``get_azimuth()``, `get_elevation()``
         and ``_get_name()``
        :param device: object with the ``GenericDerotator`` interface
        :param setupPosition: the device position commanded during the setup
        :type setupPosition: double
        :raises: PositionerError
        """
        try:
            Positioner.generalLock.acquire()
            if self.isUpdating():
                self.stopUpdating()
            self.control = Control()
            self.siteInfo = siteInfo
            self.source = source
            self.device = device
            self.device.setup()
            self._clearOffset()
            self.is_setup = True
            time.sleep(0.4) # Give the device the time to accomplish the setup
            self.control.updateScanInfo({'iStaticPos': setupPosition})
            self._setPosition(setupPosition)
        except (DerotatorErrors.PositioningErrorEx, DerotatorErrors.CommunicationErrorEx) as ex:
            raise PositionerError("cannot set the position: %s" %ex)
        except Exception as ex:
            raise PositionerError(ex)
        finally:
            Positioner.generalLock.release()


    def isSetup(self):
        return self.is_setup


    def isConfigured(self):
        return self.conf.isConfigured()


    def getConfiguration(self):
        return self.conf.getConfiguration()


    def getDeviceName(self):
        if self.isSetup():
            return self.device._get_name()
        else:
            raise NotAllowedError('positioner not configured: a setup() is required')


    def isReady(self):
        """Return True when the device is ready to move"""
        return self.isSetup() and self.device.isReady()


    def isSlewing(self):
        return self.isSetup() and self.device.isSlewing()


    def isTracking(self):
        return self.isSetup() and \
               self.device.isTracking() and not \
               self.isRewinding() and not \
               self.isRewindingRequired()


    def setPosition(self, position):
        if not self.isReady():
            raise NotAllowedError('positioner not ready, a setup() is required')
        elif not self.isConfigured():
            raise NotAllowedError('positioner not configured, a setConfiguration() is required')
        elif self.conf.getAttribute('SetCustomPositionAllowed') != 'true':
            raise NotAllowedError('setPosition() not allowed in %s configuration" %self.getConfiguration()')
        elif self.isUpdating():
            raise NotAllowedError('the positioner is executing a scan')
        elif not self.device.getMinLimit() < position < self.device.getMaxLimit():
            raise NotAllowedError('position %.2f out of range [%.2f, %.2f]'
                    %(position, self.device.getMinLimit(), self.device.getMaxLimit()))
        else:
            try:
                self.goTo(position)
                # Set the initialPosition, in order to add it to the dynamic one
                self.control.user_position = position
            except Exception as ex:
                raise PositionerError('cannot set the position: %s' %ex)


    def _setPosition(self, position):
        self.control.target = position + self.control.offset
        if self.device.getMinLimit() < self.control.target < self.device.getMaxLimit():
            try:
                self.device.setPosition(self.control.target)
            except (DerotatorErrors.PositioningErrorEx, DerotatorErrors.CommunicationErrorEx) as ex:
                raeson = "cannot set the %s position" %self.device._get_name()
                logger.logError('%s: %s' %(raeson, ex))
                raise PositionerError(raeson)
            except Exception as ex:
                raeson = "unknown exception setting the %s position" %self.device._get_name()
                logger.logError('%s: %s' %(raeson, ex))
                raise PositionerError(raeson)
        else:
            raise OutOfRangeError("position %.2f out of range {%.2f, %.2f}" 
                %(self.control.target, self.device.getMinLimit(), self.device.getMaxLimit()))


    def startUpdating(self, axis, sector, az, el, ra, dec):
        sectors = ('ANT_NORTH', 'ANT_SOUTH')
        try:
            Positioner.generalLock.acquire()
            if not self.isSetup():
                raise NotAllowedError('positioner not configured: a setup() is required')
            elif not self.conf.isConfigured():
                raise NotAllowedError('CDB not configured: a CDBConf.setConfiguration() is required')
            elif str(axis) == 'MNG_BEAMPARK':
                pass # Do nothing
            elif self.conf.getAttribute('DynamicUpdatingAllowed') != 'true':
                logger.logNotice('dynamic updating not allowed')
            elif not self.siteInfo:
                raise NotAllowedError('no site information available')
            elif not self.source:
                raise NotAllowedError('no source available')
            elif str(sector) not in sectors:
                raise NotAllowedError('sector %s not in %s' %(sector, sectors))
            else:
                if self.isUpdating():
                    self.stopUpdating()
                try:
                    updatingConf = self.conf.getUpdatingConfiguration(str(axis))
                    initialPosition = float(updatingConf['initialPosition']) + self.control.user_position
                    functionName = updatingConf['functionName']
                    # If the functionName is a staticX, we command just one position
                    if functionName.startswith('static'):
                        # functionName = 'static2' -> staticValue = float('2')
                        staticValue = float(functionName.lstrip('static')) 
                        position = initialPosition + staticValue
                        self.control.setScanInfo(
                            axis=axis, 
                            sector=sector,
                            iStaticPos=initialPosition, 
                            iParallacticPos=staticValue,
                            dParallacticPos=0,
                            rewindingOffset=0,
                        )
                        self._setPosition(position)
                        self.control.mustUpdate = False
                    else:
                        posgen = getattr(self.posgen, functionName) 
                        angle_mapping = self.posgen.mapping[functionName]
                        getAngleFunction = self.posgen.mapping[functionName]['getAngleFunction']
                        coordinateFrame = self.posgen.mapping[functionName]['coordinateFrame']
                        lat = self.siteInfo['latitude']
                        if coordinateFrame == 'horizontal':
                            iParallacticPos = getAngleFunction(lat, az, el)
                        elif coordinateFrame == 'equatorial':
                            iParallacticPos = getAngleFunction(lat, az, el, ra, dec)
                        else:
                            raise PositionerError('coordinate frame %s unknown' %coordinateFrame)

                        self.control.setScanInfo(
                            axis=axis, 
                            sector=sector,
                            iStaticPos=initialPosition, 
                            iParallacticPos=iParallacticPos,
                            dParallacticPos=0,
                            rewindingOffset=0,
                        )
                        self._start(
                            posgen,
                            self.source,
                            self.siteInfo
                        )
                        self.control.mustUpdate = True
                except Exception as ex:
                    raise PositionerError('configuration problem: %s' %ex)
        finally:
            Positioner.generalLock.release()


    def _updatePosition(self, posgen, source, siteInfo):
        try:
            self.control.isRewindingRequired = False
            self.control.isRewinding = False
            self.control.isUpdating = True
            self.control.scanInfo.update({'rewindingOffset': 0})

            if not self.device.isReady():
                logger.logError("%s not ready to move: a setup() is required" 
                        %self.device._get_name())
                return
 
            if self.isConfigured():
                isOptimized = (self.conf.getAttribute('AddInitialParallactic') == 'false')
            else:
                isOptimized = False

            new_trajectory = True
            while True:
                if self.control.stop:
                    break
                else:
                    try:
                        if new_trajectory:
                            t = getTimeStamp().value + self.trackingLeadTime
                        position = posgen(source, siteInfo, t)
                        Pis = self.control.scanInfo['iStaticPos'] + self.control.scanInfo['rewindingOffset'] 
                        Pip = self.control.scanInfo['iParallacticPos']
                        Pdp = position - Pip
                        target = Pis + Pdp if isOptimized else Pis + Pip + Pdp
                        self.control.scanInfo.update({'dParallacticPos': Pdp})

                        self.control.target = target + self.control.offset
                        if self.device.getMinLimit() < self.control.target < self.device.getMaxLimit():
                            try:
                                self.device.loadTrackingPoint(t, self.control.target, new_trajectory)
                                new_trajectory = False
                                t += self.updatingTime
                            except (DerotatorErrors.PositioningErrorEx, DerotatorErrors.CommunicationErrorEx) as ex:
                                raeson = "cannot set the %s position" %self.device._get_name()
                                logger.logError('%s: %s' %(raeson, ex))
                                raise PositionerError(raeson)
                            except Exception as ex:
                                raeson = "unknown exception setting the %s position" %self.device._get_name()
                                logger.logError('%s: %s' %(raeson, ex))
                                raise PositionerError(raeson)
                        else:
                            raise OutOfRangeError("position %.2f out of range {%.2f, %.2f}" 
                                %(self.control.target, self.device.getMinLimit(), self.device.getMaxLimit()))
                        # We calculate the time to sleep
                        # Next point of the trajectory - TrackingLeadTime - now
                        # slightly less than UpdatingTime
                        # The resulting cycle should be around
                        # TrackingLeadTime seconds before the next point
                        time_to_sleep = max(0, t - self.trackingLeadTime - getTimeStamp().value)
                        time.sleep(float(time_to_sleep) / 10**7)
                    except OutOfRangeError as ex:
                        new_trajectory = True
                        logger.logInfo(ex)
                        self.control.isRewindingRequired = True
                        if self.control.modes['rewinding'] == 'AUTO':
                            try:
                                self.rewind() 
                            except Exception as ex:
                                # In case of wrong autoRewindingSteps
                                self.control.isRewindingRequired = True
                                logger.logError('cannot rewind: %s' %ex)
                                break
                        else:
                            if self.control.modes['rewinding'] == 'MANUAL':
                                logger.logInfo('a derotator rewinding is required')
                                while self.control.isRewindingRequired:
                                    if self.control.stop:
                                        break
                                    time.sleep(0.1) # Wait until the user calls a rewind
                            else:
                                logger.logError('wrong rewinding mode: %s' %self.control.modes['rewinding'])
                    except Exception as ex:
                        logger.logError(ex)
                        break
            self.control.mustUpdate = False
        except KeyboardInterrupt:
            logger.logInfo('stopping Positioner._updatePosition() due to KeyboardInterrupt')
        except AttributeError as ex:
            logger.logError('Positioner._updatePosition(): attribute error')
            logger.logDebug('Positioner._updatePosition(): %s' %ex)
        except PositionerError as ex:
            logger.logError('Positioner._updatePosition(): %s' %ex)
        except Exception as ex:
            logger.logError('unexcpected exception in Positioner._updatePosition(): %s' %ex)
        finally:
            self.control.scanInfo.update({'rewindingOffset': 0})
            self.control.stopUpdating()


    def getScanInfo(self):
        return self.control.scanInfo


    def rewind(self, steps=None):
        if not self.isSetup():
            raise NotAllowedError('positioner not configured: a setup() is required')
        elif self.isRewinding():
            raise NotAllowedError('another rewinding is active')
        elif not self.isRewindingRequired():
            raise NotAllowedError('no rewinding is required')

        try:
            Positioner.rewindingLock.acquire()
            self.control.isRewinding = True
            # getAutoRewindingSteps() returns None in case the user did not specify it
            n = steps if steps != None else self.control.autoRewindingSteps
            targetPos, rewindingOffset = self.getRewindingParameters(n)
            self.control.updateScanInfo({'rewindingOffset': rewindingOffset})
            self._setPosition(targetPos + rewindingOffset)
            logger.logInfo('rewinding in progress...')
            startingTime = now = datetime.datetime.now()
            while (now - startingTime).seconds < float(self.conf.getAttribute('RewindingTimeout')):
                if self.control.stop:
                    break
                elif self.device.isTracking():
                    break
                else:
                    time.sleep(float(self.conf.getAttribute('RewindingSleepTime')))
                    now = datetime.datetime.now()
            else:
                raise PositionerError('%ss exceeded' %self.conf.getAttribute('RewindingTimeout'))

            self.control.isRewindingRequired = False
        except Exception as ex:
            self.control.isRewindingRequired = True
            raise PositionerError(ex)
        finally:
            self.control.isRewinding = False
            Positioner.rewindingLock.release()


    def getRewindingParameters(self, steps=None):
        """Return the target position and the rewinding offset"""
        if not self.isSetup():
            raise NotAllowedError('positioner not configured: a setup() is required')

        if steps != None and steps <= 0:
            raise PositionerError('the number of steps must be positive')

        target = self.control.target
        # The rewinding angle depends of the antenna sector: 
        #
        #   * SOUTH: the derotator must go closer to the lower limit
        #   * NORTH: the derotator must go closer to the higher limit
        #
        # The name space is the distance between the target position and the
        # limit (lower in case of SOUTH, higher in case of NORTH)
        min_limit = self.device.getMinLimit()
        max_limit = self.device.getMaxLimit()
        # To be sure, we add or subtract a delta of 1 to the target
        delta = 1.0
        sector = self.control.scanInfo['sector']
        if sector == Antenna.ANT_SOUTH:
            space = abs(target - min_limit)
            if target + delta >= max_limit:
                sign = -1
                numberOfSteps = int(space // self.device.getStep())
            elif target - delta <= min_limit:
                sign = +1
                numberOfSteps = int(space // self.device.getStep()) + 1
            else:
                raise ValueError('cannot get the rewinding parameters: ' \
                                 'target %.2f in range' %target)
        elif sector == Antenna.ANT_NORTH:
            space = abs(target - max_limit)
            if target + delta >= max_limit:
                sign = -1
                numberOfSteps = int(space // self.device.getStep()) + 1
            elif target - delta <= min_limit:
                sign = +1
                numberOfSteps = int(space // self.device.getStep())
            else:
                raise ValueError('cannot get the rewinding parameters: ' \
                                 'target %.2f in range' %target)
        else:
            raise ValueError('sector %s unknown' %sector)

        if steps == None:
            n = numberOfSteps
        elif min_limit < target + sign*steps*self.device.getStep() < max_limit:
            n = steps
        else:
            raise PositionerError(
                    'target pos: {%.1f} -> rewind of {%d} steps not allowed' \
                    %(target, steps))

        rewinding_value = sign * n * self.device.getStep()
        return (target, rewinding_value)


    def getRewindingStep(self):
        if not self.isSetup():
            raise NotAllowedError('positioner not configured: a setup() is required')
        else:
            return self.device.getStep()


    def getMaxRewindingSteps(self):
        if not self.isSetup():
            raise NotAllowedError('positioner not configured: a setup() is required')
        else:
            full_range = self.device.getMaxLimit() - self.device.getMinLimit()
            return int(full_range // self.device.getStep())


    def stopUpdating(self):
        """Stop the updating thread"""
        try:
            self.control.stop = True
            self.t.join(timeout=2)
            if self.t.isAlive():
                logger.logWarning('thread %s is alive' %self.t.getName())
        except AttributeError:
            pass # self.t is None because the system is not yet configured
        finally:
            self.control.stopUpdating()
            time.sleep(0.1)


    def park(self, parkPosition=0):
        self._clearOffset()
        if self.isSetup() and self.isUpdating():
            self.stopUpdating()
        elif self.isSetup():
            try:
                Positioner.generalLock.acquire()
                self.control.updateScanInfo({'iStaticPos': parkPosition})
                self._setPosition(parkPosition)
            finally:
                Positioner.generalLock.release()
                time.sleep(0.5) # Wait the thread stops before to set the defaults
        self._setDefault()


    def setRewindingMode(self, mode):
        modes = ('AUTO', 'MANUAL')
        if mode not in modes:
            raise PositionerError('mode %s unknown; allowed modes: %s' %(mode, modes))
        else:
            self.control.modes['rewinding'] = mode
        if mode == 'MANUAL':
            self.clearAutoRewindingSteps()


    def isSetupForRewinding(self):
        """Return True if a rewinding mode has been selected"""
        return bool(self.control.modes['rewinding'])


    def getRewindingMode(self):
        return self.control.modes['rewinding']


    def mustUpdate(self):
        return self.control.mustUpdate


    def isTerminated(self):
        if not self.t:
            return True
        else:
            return not self.isUpdating() and not self.t.isAlive() 


    def isUpdating(self):
        return self.control.isUpdating


    def goTo(self, position):
        if not self.isSetup():
            raise NotAllowedError('positioner not configured: a setup() is required')
        else:
            try:
                Positioner.generalLock.acquire()
                self.stopUpdating()
                self.control.updateScanInfo({'iStaticPos': position})
                self._setPosition(position)
            finally:
                Positioner.generalLock.release()


    def setOffset(self, offset):
        if not self.isSetup():
            raise NotAllowedError('positioner not configured: a setup() is required')
        else:
            self._setOffset(offset) # set the flag
            if self.isUpdating():
                return
            else:
                self.stopUpdating()
                actPosition = self.getPosition()
                self._setPosition(actPosition)


    def clearOffset(self):
        self.setOffset(0.0)


    def _setOffset(self, offset):
        self.control.offset = offset


    def _clearOffset(self):
        self._setOffset(0.0)


    def getOffset(self):
        return self.control.offset


    def getRewindingOffset(self):
        return self.control.scanInfo['rewindingOffset']


    def isRewindingRequired(self):
        return self.control.isRewindingRequired and not self.control.isRewinding


    def isRewinding(self):
        return self.control.isRewinding


    def getPosition(self):
        if self.isSetup():
            return self.device.getActPosition()
        else:
            raise NotAllowedError('positioner not configured: a setup() is required')


    def getMaxLimit(self):
        if self.isSetup():
            return self.device.getMaxLimit()
        else:
            raise NotAllowedError('positioner not configured: a setup() is required')


    def getMinLimit(self):
        if self.isSetup():
            return self.device.getMinLimit()
        else:
            raise NotAllowedError('positioner not configured: a setup() is required')


    def getPositionFromHistory(self, t):
        if self.isSetup():
            return self.device.getPositionFromHistory(t)
        else:
            raise NotAllowedError('positioner not configured: a setup() is required')


    def getCmdPosition(self):
        if self.isSetup():
            return self.control.target
        else:
            raise NotAllowedError('positioner not configured: a setup() is required')


    def setAutoRewindingSteps(self, steps):
        if self.isSetup():
            max_rewinding_steps = self.getMaxRewindingSteps()
            if steps > max_rewinding_steps:
                raise NotAllowedError('max number of steps: {%d} (%s given)'
                        %(max_rewinding_steps, steps))
            elif steps <= 0:
                raise NotAllowedError('the number of steps must be positive')
            else:
                self.control.autoRewindingSteps = steps
        else:
            raise NotAllowedError('positioner not configured: a setup() is required')


    def getAutoRewindingSteps(self):
        # Return 0 if self.control.autoRewindingSteps is None
        return self.control.autoRewindingSteps or 0


    def clearAutoRewindingSteps(self):
        self.control.autoRewindingSteps = None

    def _start(self, posgen, source, siteInfo):
        """Start a new process that computes and sets the position"""
        if self.isSetup():
            # self.stopUpdating() # Raise a PositionerError if the process stills alive
            self.t = ContainerServices.ContainerServices().getThread(
                    name=posgen.__name__, 
                    target=self._updatePosition, 
                    args=(posgen, source, siteInfo)
            )
            self.t.start()
            # time.sleep(0.10) # In case of goto, take the time to command the position
        else:
            raise NotAllowedError('not configured: a setConfiguration() is required')

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
            Positioner.generalLock.acquire()
            if not self.isSetup():
                raise NotAllowedError('positioner not configured: a setup() is required')

            failure = False
            warning = False
            if self.mustUpdate() and not self.isUpdating():
                failure = True

            try:
                status_obj = self.device._get_status()
            except Exception as ex:
                raise PositionerError('cannot get the device status property: %s' %ex)

            try:
                device_status, compl = status_obj.get_sync()
            except Exception as ex:
                raise PositionerError('cannot get the device status value: %s' %ex)

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
                except Exception as ex:
                    raise PositionerError('error in Status.dec2bin(): %s' %ex)

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
        except NotAllowedError as ex:
            return '000000' # Not ready
        except Exception as ex:
            logger.logError(ex)
            return '100000' # Failure
        finally:
            Positioner.generalLock.release()

    def _setDefault(self):
        self.t = None
        self.is_setup = False
        self.control = Control()
        self.conf.clearConfiguration()
        self._clearOffset()


class Control(object):
    def __init__(self):
        self.clearScanInfo()
        self.autoRewindingSteps = None
        self.modes = {'rewinding': 'none', 'updating': 'none'}
        self.stopUpdating()

    def setScanInfo(self, axis, sector, iStaticPos, iParallacticPos, dParallacticPos, rewindingOffset):
        self.scanInfo = {
            'timestamp': getTimeStamp().value,
            'axis': axis,
            'sector': sector,
            'iStaticPos': iStaticPos,
            'iParallacticPos': iParallacticPos,
            'dParallacticPos': dParallacticPos,
            'rewindingOffset': rewindingOffset
        }

    def clearScanInfo(self):
        self.offset = 0.0
        self.target = 0.0
        self.user_position = 0
        self.setScanInfo(
            axis=Management.MNG_NO_AXIS, 
            sector=Antenna.ANT_NORTH,
            iStaticPos=0,
            iParallacticPos=0,
            dParallacticPos=0,
            rewindingOffset=0,
        )

    def updateScanInfo(self, info):
        for key in info.keys():
            if key not in self.scanInfo:
                raise PositionerError('key %s not in control.scanInfo' %key)
        info.update({'timestamp': getTimeStamp().value})
        self.scanInfo.update(info)

    def stopUpdating(self):
        self.updateScanInfo({
            'axis': Management.MNG_NO_AXIS,
            'iParallacticPos': 0,
            'dParallacticPos': 0,
            'rewindingOffset': 0,
        })
        self.isRewindingRequired = False
        self.isRewinding = False
        self.isUpdating = False
        self.mustUpdate = False
        self.stop = False


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
            '10000'
            >>> Status.dec2bin(16, 10)
            '0000010000'
            >>> Status.dec2bin(0, 6)
            '000000'
            >>> Status.dec2bin(32, 6)
            '100000'
        """
        if n == 0:
            value = '0' 
        else:
            value = Status.dec2bin(n//2) + str(n%2)

        if nbits:
            if len(value) > nbits:
                return value[-nbits:]
            elif len(value) == nbits:
                return value
            else:
                return '0'*(nbits - len(value)) + value
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
