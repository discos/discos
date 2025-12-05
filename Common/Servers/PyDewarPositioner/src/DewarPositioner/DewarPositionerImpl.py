from __future__ import print_function
import time
from math import radians
from multiprocessing import Queue
from Receivers__POA import DewarPositioner as POA
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as cc
from Acspy.Servants.ContainerServices import ContainerServices as services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as lcycle
from Acspy.Util.BaciHelper import addProperty
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Nc.Supplier import Supplier
from Acspy.Common.TimeHelper import getTimeStamp
from maciErrType import CannotGetComponentEx
from ACSErrTypeCommonImpl import CORBAProblemExImpl

import Management
import Receivers 
import ComponentErrorsImpl
import ComponentErrors
import DerotatorErrors 


from DewarPositioner.cdbconf import CDBConf
from DewarPositioner.positioner import Positioner, PositionerError, NotAllowedError
from DewarPositioner.devios import StatusDevIO
from Receivers import TDewarPositionerScanInfo as ScanInfo

from IRAPy import logger
from ZMQLibrary import ZMQPublisher, ZMQTimeStamp


__copyright__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class DewarPositionerImpl(POA, cc, services, lcycle):
 
    commands = {
        # command_name: (method_name, (type_of_arg1, ..., type_of_argN))
        'derotatorSetup': ('setup', (str,)), 
        'derotatorGetActualSetup': ('getActualSetup', ()),
        'derotatorIsReady': ('isReady', ()), 
        'derotatorSetConfiguration': ('setConfiguration', (str,)),
        'derotatorGetConfiguration': ('getConfiguration', ()), 
        'derotatorPark': ('park', ()),
        'derotatorSetOffset': ('setOffset', (float,)),
        'derotatorGetOffset': ('getOffset', ()),
        'derotatorClearOffset': ('clearOffset', ()),
        'derotatorSetRewindingMode': ('setRewindingMode', (str,)),
        'derotatorGetRewindingMode': ('getRewindingMode', ()),
        'derotatorSetAutoRewindingSteps': ('setAutoRewindingSteps', (int,)),
        'derotatorGetAutoRewindingSteps': ('getAutoRewindingSteps', ()),
        'derotatorClearAutoRewindingSteps': ('clearAutoRewindingSteps', ()),
        'derotatorIsUpdating': ('isUpdating', ()),
        'derotatorIsRewinding': ('isRewinding', ()), 
        'derotatorIsRewindingRequired': ('isRewindingRequired', ()), 
        'derotatorRewind': ('rewind', (int,)),
        'derotatorSetPosition': ('_setPositionCmd', (str,)),
        'derotatorGetPosition': ('_getPositionCmd', ()), 
        'derotatorGetCmdPosition': ('_getCmdPositionCmd', ()), 
        'derotatorGetRewindingStep': ('_getRewindingStepCmd', ()),
        'derotatorGetScanInfo': ('_getScanInfoCmd', ()),
        'derotatorGetMaxLimit': ('_getMaxLimitCmd', ()),
        'derotatorGetMinLimit': ('_getMinLimitCmd', ()),
    }


    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self.cdbconf = CDBConf()
        self.positioner = Positioner(self.cdbconf)
        self._setDefaultSetup() 
        self.client = PySimpleClient()
        self.control = Control()
        self.zmqDictionary = {
            "currentConfiguration": "",
            "currentDerotator": "",
            "currentSetup": "",
            "rewinding": False,
            "rewindingMode": "",
            "rewindingRequired": False,
            "status": "WARNING",
            "timestamp": ZMQTimeStamp.fromUNIXTime(time.time()),
            "tracking": False
        }
        try:
            self.supplier = Supplier(Receivers.DEWAR_POSITIONER_DATA_CHANNEL)
        except CORBAProblemExImpl as ex:
            logger.logError('cannot create the dewar positioner data channel')
            logger.logDebug('cannot create the data channel: %s' %ex)
        except Exception as ex:
            reason = ex.getReason() if hasattr(ex, 'getReason') else ex
            logger.logError(reason)

        try:
            self.statusThread = services().getThread(
                    name='Publisher',
                    target=DewarPositionerImpl.publisher,
                    args=(
                        self.positioner,
                        self.supplier,
                        self.control,
                        self.zmqDictionary
                    )
            )
            self.statusThread.start()
        except AttributeError as ex:
            logger.logWarning('supplier not available')
            logger.logDebug('supplier not available: %s' %ex)
        except Exception as ex:
            logger.logError('cannot create the status thread: %s' %ex)

    def initialize(self):
        addProperty(self, 'status', devio_ref=StatusDevIO(self.positioner))

    def cleanUp(self):
        try:
            self.control.stop = True
            self.supplier.disconnect()
            self.statusThread.join(timeout=5)
            if self.statusThread.isAlive():
                logger.logError('thread %s is alive' %self.statusThread.getName())
        except AttributeError as ex:
            logger.logDebug('self has no attribute `supplier`: %s' %ex)
        except Exception as ex:
            logger.logError(ex)
        finally:
            self.control.stop = False


    def setup(self, code):
        self.commandedSetup = code.upper()
        logger.logNotice('starting the derotator %s setup' %self.commandedSetup)
        deviceName = ""
        try:
            self.cdbconf.setup(self.commandedSetup)
            deviceName = self.cdbconf.getAttribute('DerotatorName')
            device = self.client.getComponent(deviceName)
        except CannotGetComponentEx as ex:
            reason = "cannot get the %s component: %s" %(deviceName, ex)
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()

        try:
            observatoryName = self.cdbconf.getAttribute('ObservatoryName')
            observatory = self.client.getComponent(observatoryName)
            lat_obj = observatory._get_latitude()
            latitude_dec, compl = lat_obj.get_sync()
            latitude = radians(latitude_dec)
        except Exception as ex:
            reason = ex.getReason() if hasattr(ex, 'getReason') else ex
            logger.logWarning('cannot get the site information: %s' %reason)
            latitude = radians(float(self.cdbconf.getAttribute('Latitude')))
            logger.logWarning('setting the default latitude value: %.2f radians' %latitude)
        finally:
            siteInfo = {'latitude': latitude}

        try:
            sourceName = self.cdbconf.getAttribute('CoordinateSourceName')
            source = self.client.getComponent(sourceName)
        except Exception:
            logger.logWarning('cannot get the %s component' %sourceName)
            source = None

        try:
            self.positioner.setup(
                    siteInfo, 
                    source, 
                    device, 
                    float(self.cdbconf.getAttribute('SetupPosition')))
                
            self.setConfiguration(self.cdbconf.getAttribute('DefaultConfiguration'))
            self.setRewindingMode(self.cdbconf.getAttribute('DefaultRewindingMode'))
            self.actualSetup = self.commandedSetup
            self.zmqDictionary["currentDerotator"] = deviceName.split("/")[-1]
            self.zmqDictionary["currentSetup"] = self.actualSetup
            logger.logNotice('derotator %s setup done' %self.commandedSetup)
        except PositionerError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()
        except ComponentErrors.ComponentErrorsEx as ex:
            data_list = ex.errorTrace.data # A list
            reason = data_list[0].value if data_list else 'component error'
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            reason = ex.getReason() if hasattr(ex, 'getReason') else ex
            logger.logError(reason)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', reason)
            raise exc.getComponentErrorsEx()

    def park(self):
        logger.logNotice('parking the derotator')
        try:
            if self.positioner.isReady():
                parkPosition = float(self.cdbconf.getAttribute('ParkPosition'))
                self.positioner.park(parkPosition)
            else:
                logger.logWarning('positioner not ready: a setup() is required')
                self.positioner.park()
        except NotAllowedError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()
        finally:
            self._setDefaultSetup()
        self.zmqDictionary["currentSetup"] = ""
        self.zmqDictionary["currentDerotator"] = ""
        logger.logNotice('derotator parked')


    def getPosition(self):
        try:
            return self.positioner.getPosition()
        except NotAllowedError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()
        except (DerotatorErrors.CommunicationErrorEx, ComponentErrors.ComponentErrorsEx) as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason("Cannot get the derotator position")
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()


    def getMaxLimit(self):
        try:
            return self.positioner.getMaxLimit()
        except NotAllowedError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()


    def getMinLimit(self):
        try:
            return self.positioner.getMinLimit()
        except NotAllowedError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()


    def getPositionFromHistory(self, t):
        try:
            return self.positioner.getPositionFromHistory(t)
        except NotAllowedError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()
        except (DerotatorErrors.CommunicationErrorEx, ComponentErrors.ComponentErrorsEx) as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason("Cannot get the derotator position at the time %s" %s)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()

    def getCmdPosition(self):
        try:
            return self.positioner.getCmdPosition()
        except NotAllowedError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()


    def setPosition(self, position):
        try:
            self.positioner.setPosition(position)
        except NotAllowedError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()


    def _setPositionCmd(self, position):
        """Wrap setPosition() in order to strip the `d` at the end of the string"""
        str_value = position.strip().rstrip('d')
        self.setPosition(float(str_value))
 

    def _getPositionCmd(self):
        """Wrap getPosition() in order to add the `d` at the end of the string"""
        return '%.4fd' %self.getPosition()
 

    def _getCmdPositionCmd(self):
        """Wrap getCmdPosition() in order to add the `d` at the end of the string"""
        return '%.4fd' %self.getCmdPosition()
 

    def _getMaxLimitCmd(self):
        """Wrap getMaxLimit() in order to add the `d` at the end of the string"""
        return '%.4fd' %self.getMaxLimit()
 

    def _getMinLimitCmd(self):
        """Wrap getMinLimit() in order to add the `d` at the end of the string"""
        return '%.4fd' %self.getMinLimit()
 

    def _getRewindingStepCmd(self):
        """Wrap getRewindingStep() in order to add the `d` at the end of the string"""
        return '%sd' %self.getRewindingStep()
    

    def _getScanInfoCmd(self):
        """Wrap getScanAxis() in order to show a human representation""" 
        res = ''
        info = self.getScanInfo()
        res += 'axis: %s\n' %info.axis
        res += 'sector: %s\n' %info.sector
        res += 'Pis: %.4f\n' %info.iStaticPos
        res += 'Pip: %.4f\n' %info.iParallacticPos
        res += 'Pdp: %.4f\n' %info.dParallacticPos
        res += 'rewindingOffset: %.4f' %info.rewindingOffset
        return res
        
    def getScanInfo(self):
        return ScanInfo(**self.positioner.getScanInfo())

    def startUpdating(self, axis, sector, az, el, ra, dec):
        logger.logNotice('starting the derotator position updating')
        try:
            self.positioner.startUpdating(axis, sector, az, el, ra, dec)
        except PositionerError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()
        except NotAllowedError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl(ex)
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()

    def stopUpdating(self):
        logger.logNotice('stopping the derotator position updating')
        try:
            self.positioner.stopUpdating()
        except PositionerError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()
        logger.logNotice('derotator position updating stopped')


    def setAutoRewindingSteps(self, steps):
        try:
            self.positioner.setAutoRewindingSteps(steps)
        except NotAllowedError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()


    def getAutoRewindingSteps(self):
        return self.positioner.getAutoRewindingSteps()


    def clearAutoRewindingSteps(self):
        return self.positioner.clearAutoRewindingSteps()


    def rewind(self, steps):
        try:
            return self.positioner.rewind(steps)
        except (PositionerError, NotAllowedError) as ex:
            reason = "cannot rewind the derotator: %s" %ex
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()


    def isRewinding(self):
        return self.positioner.isRewinding()


    def isRewindingRequired(self):
        return self.positioner.isRewindingRequired()


    def getRewindingStep(self):
        try:
            return self.positioner.getRewindingStep()
        except NotAllowedError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()


    def isConfigured(self):
        # Add the isReady() constraint because the Receiver boss checks if
        # the DewarPositioner is configured instead of checking if it is Ready
        return self.positioner.isConfigured() and self.isReady()


    def isReady(self):
        try:
            return self.positioner.isReady()
        except DerotatorErrors.CommunicationErrorEx as ex:
            reason = "cannot known if the derotator is ready: %s" %ex
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()

    def isSlewing(self):
        try:
            return self.positioner.isSlewing()
        except DerotatorErrors.CommunicationErrorEx as ex:
            reason = "cannot known if the derotator is slewing: %s" %ex
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()

    def isTracking(self):
        try:
            return self.positioner.isTracking()
        except DerotatorErrors.CommunicationErrorEx as ex:
            reason = "cannot known if the derotator is tracking: %s" %ex
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()

    def isUpdating(self):
        return self.positioner.isUpdating()

    def setOffset(self, offset):
        try:
            self.positioner.setOffset(offset)
        except (PositionerError, NotAllowedError) as ex:
            reason = "cannot set the derotator offset: %s" %ex
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except NotAllowedError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()

    def clearOffset(self):
        try:
            self.positioner.clearOffset()
        except PositionerError as ex:
            reason = "cannot set the derotator offset: %s" %ex
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except NotAllowedError as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex)
            raise exc.getComponentErrorsEx()
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex)
            raise exc.getComponentErrorsEx()


    def getOffset(self):
        return self.positioner.getOffset()


    def getActualSetup(self):
        return self.actualSetup


    def getCommandedSetup(self):
        return self.commandedSetup


    def setRewindingMode(self, mode):
        try:
            mode = mode.upper().strip()
            self.positioner.setRewindingMode(mode)
        except PositionerError as ex:
            reason = 'cannot set the rewinding mode: %s' %ex
            logger.logError(reason)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx() # wrong system input


    def setConfiguration(self, confCode):
        if self.positioner.isUpdating():
            self.positioner.stopUpdating()
        # The unit of time is `seconds`
        max_wait_time = 2 
        counter = 0
        step = 0.2
        while self.positioner.isUpdating():
            self.positioner.stopUpdating()
            time.sleep(step)
            counter += step
            if counter > max_wait_time:
                reason = "cannot stop the position updating"
                logger.logError(reason)
                exc = ComponentErrorsImpl.OperationErrorExImpl()
                exc.setReason(reason)
                raise exc.getComponentErrorsEx()
        try:
            self.positioner.control.clearScanInfo()
            self.cdbconf.setConfiguration(confCode.upper())
        except Exception as ex:
            reason = ex.getReason() if hasattr(ex, 'getReason') else ex
            logger.logError(reason)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', reason)
            raise exc.getComponentErrorsEx()


    def getManagementStatus(self):
        return self.control.mngStatus

    def getConfiguration(self):
        return self.positioner.getConfiguration()

    def getRewindingMode(self):
        return self.positioner.getRewindingMode()

    def _setDefaultSetup(self):
        self.actualSetup = 'none'
        self.commandedSetup = ''

    @staticmethod
    def publisher(positioner, supplier, control, zmqDictionary, sleep_time=1):
        zmqPublisher = ZMQPublisher("derotators")

        error = False
        while True:
            if control.stop:
                break
            else:
                try:
                    status = [bool(int(item)) for item in positioner.getStatus()]
                    failure, warning, slewing, updating, tracking, ready = status

                    if failure:
                        control.mngStatus = Management.MNG_FAILURE
                        zmqDictionary["status"] = "FAILURE"
                    elif warning or not positioner.isSetup():
                        control.mngStatus = Management.MNG_WARNING
                        zmqDictionary["status"] = "WARNING"
                    else:
                        control.mngStatus = Management.MNG_OK
                        zmqDictionary["status"] = "OK"

                    event = Receivers.DewarPositionerDataBlock(
                            getTimeStamp().value,
                            ready,
                            tracking, 
                            updating,
                            slewing,
                            control.mngStatus
                    )
                    supplier.publishEvent(simple_data=event)
                    error = False

                    if zmqDictionary["currentSetup"] != "":
                        zmqDictionary["currentConfiguration"] = positioner.getConfiguration()
                        zmqDictionary["rewinding"] = positioner.isRewinding()
                        zmqDictionary["rewindingMode"] = positioner.getRewindingMode().upper()
                        zmqDictionary["rewindingRequired"] = positioner.isRewindingRequired()
                        zmqDictionary["tracking"] = tracking
                        zmqDictionary["updating"] = updating
                    else:
                        zmqDictionary["currentConfiguration"] = ""
                        zmqDictionary["rewinding"] = False
                        zmqDictionary["rewindingMode"] = "NONE"
                        zmqDictionary["rewindingRequired"] = False
                        zmqDictionary["tracking"] = False
                        zmqDictionary["updating"] = False

                    zmqDictionary["timestamp"] = ZMQTimeStamp.fromUNIXTime(time.time())
                    zmqPublisher.publish(zmqDictionary)
                except Exception as ex:
                    reason = ex.getReason() if hasattr(ex, 'getReason') else ex
                    if not error:
                        logger.logError('cannot publish the status: %s' %reason)
                        error = True
                finally:
                    time.sleep(sleep_time)

    def command(self, cmd):
        """Execute the command cmd
        
        This method returns a tuple (success, answer), where success is a boolean
        that indicates the command is executed correctly (success = True) or not
        (success = False). The argument answer is a string that represents the
        error message in case of error, the value returned from the method in
        case it returns a non Null object, an empty string in case it returns None.
        """
        try:
            command, args_str = cmd.split('=') if '=' in cmd else (cmd, '')
            command = command.strip()
            args = [item.strip() for item in args_str.split(',')]
        except ValueError:
            success = False 
            answer = 'Error - invalid command: maybe there are too many symbols of ='
        except Exception as ex:
            success = False 
            answer = ex
        else:
            success = True

        if not success:
            logger.logError(answer)
            return (success, answer)

        if command not in DewarPositionerImpl.commands:
            success = False
            answer = 'Error - command %s does not exist' %command
            logger.logError(answer)
            return (success, answer)
        else:
            # For instance:
            # >>> DewarPositionerImpl.commands['foo']
            # ('setup', (str, str, float))
            # >>> method_name, types = DewarPositionerImpl.commands[command]
            # >>> method_name
            # 'setup'
            # >>> types
            # (str, str, float)
            method_name, types = DewarPositionerImpl.commands[command]
            # If we expect some arguments but there is not
            if types and not any(args):
                success = False
                answer = 'Error - missing arguments, type help(%s) for details' %command
                logger.logError(answer)
                return (success, answer)

            try:
                method = getattr(self, method_name)
            except AttributeError as ex:
                success = False
                answer = "Error - %s has no attribute %s" %(self, method_name)
                logger.logError(answer)
                return (success, answer)
            try:
                result = method(*[type_(arg) for (arg, type_) in zip(args, types)])
                # For instance:
                # >>> args = ('1', 'python', '3.50')
                # >>> types = (int, str, float)
                # >>> [type_(arg) for (arg, type_) in zip(args, types)]
                # [1, 'python', 3.5]
                # >>> def foo(a, b, c):
                # ...     print(a, b, c)
                # ... 
                # >>> foo(*[type_(arg) for (arg, type_) in zip(args, types)])
                # 1 python 3.5
                answer = '' if result is None else str(result)
                success = True
            except (ValueError, TypeError) as ex:
                success = False
                answer = 'Error - wrong parameter usage.\nType help(%s) for details' %command
                logger.logError('%s\n%s' %(ex, answer))
                return (success, answer)
            except ComponentErrors.ComponentErrorsEx as ex:
                success = False
                data_list = ex.errorTrace.data # A list
                reason = data_list[0].value if data_list else 'component error'
                answer = 'Error - %s' %reason
                logger.logError(answer)
                return (success, answer)
            except Exception as ex:
                success = False
                msg = ex if ex else 'unexpected exception'
                answer = 'Error - %s' %(ex.getReason() if hasattr(ex, 'getReason') else msg)
                logger.logError(answer)
                return (success, answer)

        logger.logInfo('command %s executed' %cmd)
        return (success, answer)
        

class Control(object):
    def __init__(self):
        self.stop = False
        self.mngStatus = Management.MNG_WARNING

