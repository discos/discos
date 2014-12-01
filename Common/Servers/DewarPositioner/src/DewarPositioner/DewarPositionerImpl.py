import time
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
import DerotatorErrorsImpl
import DerotatorErrors 


from DewarPositioner.cdbconf import CDBConf
from DewarPositioner.positioner import Positioner, PositionerError, NotAllowedError
from DewarPositioner.devios import StatusDevIO

from IRAPy import logger


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
        'derotatorGetRewindingStep': ('_getRewindingStepCmd', ()),
    }

    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self.cdbconf = CDBConf()
        self.positioner = Positioner(self.cdbconf)
        self._setDefaultSetup() 
        self.client = PySimpleClient()
        self.control = Control()
        try:
            self.supplier = Supplier(Receivers.DEWAR_POSITIONER_DATA_CHANNEL)
        except CORBAProblemExImpl, ex:
            logger.logError('cannot create the dewar positioner data channel')
            logger.logDebug('cannot create the data channel: %s' %ex.message)
        except Exception, ex:
            reason = ex.getReason() if hasattr(ex, 'getReason') else ex.message
            logger.logError(reason)

        try:
            self.statusThread = services().getThread(
                    name='Publisher',
                    target=DewarPositionerImpl.publisher,
                    args=(self.positioner, self.supplier, self.control)
            )
            self.statusThread.start()
        except AttributeError, ex:
            logger.logWarning('supplier not available')
            logger.logDebug('supplier not available: %s' %ex.message)
        except Exception, ex:
            logger.logError('cannot create the status thread: %s' %ex.message)

    def initialize(self):
        addProperty(self, 'status', devio_ref=StatusDevIO(self.positioner))

    def cleanUp(self):
        try:
            self.control.stop = True
            self.supplier.disconnect()
            self.statusThread.join(timeout=5)
            if self.statusThread.isAlive():
                logger.logError('thread %s is alive' %self.statusThread.getName())
        except AttributeError, ex:
            logger.logDebug('self has no attribute `supplier`: %s' %ex.message)
        except Exception, ex:
            logger.logError(ex.message)
        finally:
            self.control.stop = False


    def setup(self, code):
        self.commandedSetup = code.upper()
        logger.logNotice('starting the derotator %s setup' %self.commandedSetup)
        try:
            self.cdbconf.setup(self.commandedSetup)
            deviceName = self.cdbconf.getAttribute('DerotatorName')
            device = self.client.getComponent(deviceName)
        except CannotGetComponentEx, ex:
            reason = "cannot get the %s component: %s" %(deviceName, ex.message)
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()

        try:
            observatory = self.client.getComponent('ANTENNA/Observatory')
            lat_obj = observatory._get_latitude()
            latitude, compl = lat_obj.get_sync()
        except Exception, ex:
            reason = ex.getReason() if hasattr(ex, 'getReason') else ex.message
            logger.logWarning('cannot get the site information: %s' %reason)
            latitude = float(self.cdbconf.getAttribute('Latitude'))
            logger.logWarning('setting the default latitude value: %.2f' %latitude)
        finally:
            siteInfo = {'latitude': latitude}

        try:
            sourceName = 'ANTENNA/Boss'
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
            logger.logNotice('derotator %s setup done' %self.commandedSetup)
        except PositionerError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except ComponentErrors.ComponentErrorsEx, ex:
            data_list = ex.errorTrace.data # A list
            reason = data_list[0].value if data_list else 'component error'
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            reason = ex.getReason() if hasattr(ex, 'getReason') else ex.message
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
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()
        finally:
            self._setDefaultSetup()
        logger.logNotice('derotator parked')

    def getPosition(self):
        try:
            return self.positioner.getPosition()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except (DerotatorErrors.CommunicationErrorEx, ComponentErrors.ComponentErrorsEx), ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason("Cannot get the derotator position")
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()


    def setPosition(self, position):
        try:
            self.positioner.setPosition(position)
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()


    def _setPositionCmd(self, position):
        """Wrap setPosition() in order to strip the `d` at the end of the string"""
        str_value = position.strip().rstrip('d')
        self.setPosition(float(str_value))
 

    def _getPositionCmd(self):
        """Wrap getPosition() in order to add the `d` at the end of the string"""
        return '%.4fd' %self.getPosition()
 

    def _getRewindingStepCmd(self):
        """Wrap getRewindingStep() in order to add the `d` at the end of the string"""
        return '%sd' %self.getRewindingStep()


    def startUpdating(self, axis, sector):
        logger.logNotice('starting the derotator position updating')
        try:
            self.positioner.startUpdating(str(axis), str(sector))
        except PositionerError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl(ex.message)
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()

    def stopUpdating(self):
        logger.logNotice('stopping the derotator position updating')
        try:
            self.positioner.stopUpdating()
        except PositionerError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()
        logger.logNotice('derotator position updating stopped')


    def setAutoRewindingSteps(self, steps):
        try:
            self.positioner.setAutoRewindingSteps(steps)
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()


    def getAutoRewindingSteps(self):
        return self.positioner.getAutoRewindingSteps()


    def clearAutoRewindingSteps(self):
        return self.positioner.clearAutoRewindingSteps()


    def rewind(self, steps):
        try:
            return self.positioner.rewind(steps)
        except (PositionerError, NotAllowedError), ex:
            reason = "cannot rewind the derotator: %s" %ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()


    def isRewinding(self):
        return self.positioner.isRewinding()


    def isRewindingRequired(self):
        return self.positioner.isRewindingRequired()


    def getRewindingStep(self):
        try:
            return self.positioner.getRewindingStep()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()


    def isConfigured(self):
        return self.positioner.isConfigured()


    def isReady(self):
        try:
            return self.positioner.isReady()
        except DerotatorErrors.CommunicationErrorEx, ex:
            reason = "cannot known if the derotator is ready: %s" %ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()

    def isSlewing(self):
        try:
            return self.positioner.isSlewing()
        except DerotatorErrors.CommunicationErrorEx, ex:
            reason = "cannot known if the derotator is slewing: %s" %ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()

    def isTracking(self):
        try:
            return self.positioner.isTracking()
        except DerotatorErrors.CommunicationErrorEx, ex:
            reason = "cannot known if the derotator is tracking: %s" %ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()

    def isUpdating(self):
        return self.positioner.isUpdating()

    def setOffset(self, offset):
        try:
            self.positioner.setOffset(offset)
        except (PositionerError, NotAllowedError), ex:
            reason = "cannot set the derotator offset: %s" %ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()

    def clearOffset(self):
        try:
            self.positioner.clearOffset()
        except PositionerError, ex:
            reason = "cannot set the derotator offset: %s" %ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
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
        except PositionerError, ex:
            reason = 'cannot set the rewinding mode: %s' %ex.message
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
        step = 0.1
        while self.positioner.isUpdating():
            time.sleep(step)
            counter += step
            if counter > max_wait_time:
                reason = "cannot stop the position updating"
                logger.logError(reason)
                exc = ComponentErrorsImpl.OperationErrorExImpl()
                exc.setReason(reason)
                raise exc.getComponentErrorsEx()
        try:
            self.cdbconf.setConfiguration(confCode.upper())
            if self.cdbconf.getAttribute('SetCustomPositionAllowed') == 'true':
                position = self.positioner.getPosition()
                # Set the initialPosition, in order to add it to the dynamic one
                self.cdbconf.updateInitialPositions(position)
                logger.logNotice('initial position set to %.4f' %position)
        except Exception, ex:
            reason = ex.getReason() if hasattr(ex, 'getReason') else ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', reason)
            raise exc.getComponentErrorsEx()


    def getConfiguration(self):
        return self.positioner.getConfiguration()

    def getRewindingMode(self):
        return self.positioner.getRewindingMode()

    def _setDefaultSetup(self):
        self.actualSetup = 'none'
        self.commandedSetup = ''

    @staticmethod
    def publisher(positioner, supplier, control, sleep_time=1):
        error = False
        while True:
            if control.stop:
                break
            else:
                try:
                    status = [bool(int(item)) for item in positioner.getStatus()]
                    failure, warning, slewing, updating, tracking, ready = status

                    if failure:
                        management_status = Management.MNG_FAILURE
                    elif warning:
                        management_status = Management.MNG_WARNING
                    else:
                        management_status = Management.MNG_OK

                    event = Receivers.DewarPositionerDataBlock(
                            getTimeStamp().value,
                            ready,
                            tracking, 
                            updating,
                            slewing,
                            management_status
                    )
                    supplier.publishEvent(simple_data=event)
                    error = False
                except Exception, ex:
                    reason = ex.getReason() if hasattr(ex, 'getReason') else ex.message
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
        except Exception, ex:
            success = False 
            answer = ex.message
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
            except AttributeError, ex:
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
                # ...     print a, b, c
                # ... 
                # >>> foo(*[type_(arg) for (arg, type_) in zip(args, types)])
                # 1 python 3.5
                answer = '' if result is None else str(result)
                success = True
            except (ValueError, TypeError), ex:
                success = False
                answer = 'Error - wrong parameter usage.\nType help(%s) for details' %command
                logger.logError('%s\n%s' %(ex.message, answer))
                return (success, answer)
            except ComponentErrors.ComponentErrorsEx, ex:
                success = False
                data_list = ex.errorTrace.data # A list
                reason = data_list[0].value if data_list else 'component error'
                answer = 'Error - %s' %reason
                logger.logError(answer)
                return (success, answer)
            except Exception, ex:
                success = False
                msg = ex.message if ex.message else 'unexpected exception'
                answer = 'Error - %s' %(ex.getReason() if hasattr(ex, 'getReason') else msg)
                logger.logError(answer)
                return (success, answer)

        logger.logInfo('command %s executed' %cmd)
        return (success, answer)

        
class Control(object):
    def __init__(self):
        self.stop = False

