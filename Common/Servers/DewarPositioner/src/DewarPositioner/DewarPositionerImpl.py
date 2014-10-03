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
        'derotatorPark': ('park', ()),
        'derotatorRewind': ('rewind', (int,)),
        'derotatorStartUpdating': ('startUpdating', ()),
        'derotatorStopUpdating': ('stopUpdating', ()),
        'derotatorSetOffset': ('setOffset', (float,)),
        'derotatorClearOffset': ('clearOffset',),
        'derotatorSetUpdatingMode': ('setUpdatingMode', (str,)),
        'derotatorClearUpdatingMode': ('clearUpdatingMode', ()),
        'derotatorSetRewindingMode': ('setRewindingMode', (str,)),
    }

    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self.cdbconf = CDBConf()

        try:
            cdb_attributes = {
                'updating_time': self.cdbconf.get_attribute('updating_time'),
                'rewinding_timeout': self.cdbconf.get_attribute('rewinding_timeout'),
                'rewinding_sleep_time': self.cdbconf.get_attribute('rewinding_sleep_time') 
            }
            self.positioner = Positioner(cdb_attributes)
        except AttributeError, ex:
            logger.logWarning('cannot get the CDB attribute %s' %ex.message)
        except Exception, ex:
            logger.logWarning('cannot get the CDB attributes: %s' %ex.message)

        self._setDefaultConfiguration() 
        self.client = PySimpleClient()
        self.control = Control()
        try:
            self.supplier = Supplier(Receivers.DEWAR_POSITIONER_DATA_CHANNEL)
        except CORBAProblemExImpl, ex:
            logger.logError('cannot create the dewar positioner data channel')
            logger.logDebug('cannot create the data channel: %s' %ex.message)
        except Exception, ex:
            logger.logError(ex.message)

        try:
            self.status_thread = services().getThread(
                    name='Publisher',
                    target=DewarPositionerImpl.publisher,
                    args=(self.positioner, self.supplier, self.control)
            )
            self.status_thread.start()
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
            self.status_thread.join(timeout=5)
            if self.status_thread.isAlive():
                logger.logError('thread %s is alive' %self.status_thread.getName())
        except AttributeError:
            logger.logDebug('self has no attribute `supplier`: %s' %ex.message)
        except Exception, ex:
            logger.logError(ex.message)
        finally:
            self.control.stop = False

    def setup(self, code):
        self.commandedSetup = code.upper()
        self.cdbconf.setup(self.commandedSetup)
        device_name = self.cdbconf.get_entry('derotator_name')
        starting_position = self.cdbconf.get_entry('starting_position') 
        try:
            device = self.client.getComponent(device_name)
        except CannotGetComponentEx, ex:
            raeson = "cannot get the %s component: %s" %(device_name, ex.message)
            logger.logError(raeson)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

        try:
            observatory = self.client.getComponent('ANTENNA/Observatory')
            lat_obj = observatory._get_latitude()
            latitude, compl = lat_obj.get_sync()
            site_info = {'latitude': latitude}
        except Exception, ex:
            logger.logWarning('cannot get the site information: %s' %ex.message)
            site_info = {}

        try:
            source_name = 'ANTENNA/Mount'
            source = self.client.getComponent(source_name)
        except Exception:
            logger.logWarning('cannot get the %s component' %source_name)
            source = None

        try:
            self.positioner.setup(site_info, source, device, starting_position)
            self.setRewindingMode('AUTO')
            self.actualSetup = self.commandedSetup
        except PositionerError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(ex.message)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def park(self):
        try:
            self.positioner.park()
            self._setDefaultConfiguration()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def getPosition(self):
        try:
            return self.positioner.getPosition()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc
        except (DerotatorErrors.CommunicationErrorEx, ComponentErrors.ComponentErrorsEx), ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason("Cannot get the derotator position")
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def startUpdating(self):
        try:
            self.positioner.startUpdating()
        except PositionerError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(ex.message)
            raise exc
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def stopUpdating(self):
        try:
            self.positioner.stopUpdating()
        except PositionerError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(ex.message)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def rewind(self, number_of_feeds):
        try:
            return self.positioner.rewind(number_of_feeds)
        except (PositionerError, NotAllowedError), ex:
            raeson = "cannot rewind the derotator: %s" %ex.message
            logger.logError(raeson)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def isRewinding(self):
        return self.positioner.isRewinding()

    def isConfigured(self):
        return self.positioner.isConfigured()

    def isConfiguredForUpdating(self):
        """Return True if an updating mode has been selected"""
        return self.positioner.isConfiguredForUpdating()

    def isConfiguredForRewinding(self):
        """Return True if a rewinding mode has been selected"""
        return self.positioner.isConfiguredForRewinding()

    def isReady(self):
        try:
            return self.positioner.isReady()
        except DerotatorErrors.CommunicationErrorEx, ex:
            raeson = "cannot known if the derotator is ready: %s" %ex.message
            logger.logError(raeson)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def isSlewing(self):
        try:
            return self.positioner.isSlewing()
        except DerotatorErrors.CommunicationErrorEx, ex:
            raeson = "cannot known if the derotator is slewing: %s" %ex.message
            logger.logError(raeson)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def isTracking(self):
        try:
            self.positioner.isTracking()
        except DerotatorErrors.CommunicationErrorEx, ex:
            raeson = "cannot known if the derotator is tracking: %s" %ex.message
            logger.logError(raeson)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def isUpdating(self):
        return self.positioner.isUpdating()

    def setOffset(self, offset):
        try:
            self.positioner.setOffset(offset)
        except (PositionerError, NotAllowedError), ex:
            raeson = "cannot set the derotator offset: %s" %ex.message
            logger.logError(raeson)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def clearOffset(self):
        try:
            self.positioner.clearOffset()
        except PositionerError, ex:
            raeson = "cannot set the derotator offset: %s" %ex.message
            logger.logError(raeson)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def getOffset(self):
        return self.positioner.getOffset()

    def getActualSetup(self):
        return self.actualSetup

    def getCommandedSetup(self):
        return self.commandedSetup

    def setUpdatingMode(self, mode):
        try:
            self.positioner.setUpdatingMode(mode.upper())
        except PositionerError, ex:
            raeson = 'cannot set the updating mode: %s' %ex.message
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc # Can happen only in case of wrong system input

    def clearUpdatingMode(self):
        self.positioner.clearUpdatingMode()

    def getUpdatingMode(self):
        return self.positioner.getUpdatingMode()

    def setRewindingMode(self, mode):
        try:
            self.positioner.setRewindingMode(mode.upper())
        except PositionerError, ex:
            raeson = 'cannot set the rewinding mode: %s' %ex.message
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc # Can happen only in case of wrong system input

    def getRewindingMode(self):
        return self.positioner.getRewindingMode()

    def _setDefaultConfiguration(self):
        self.actualSetup = 'unknown'
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
                    if not error:
                        logger.logError('cannot publish the status: %s' %ex.message)
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
            answer = 'invalid command: maybe there are too many symbols of ='
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
            answer = 'command %s does not exist' %command
            logger.logError(answer)
            return (success, answer)
        else:
            method_name, types = DewarPositionerImpl.commands[command]
            # For instance:
            # >>> DewarPositionerImpl.commands['foo']
            # ('setup', (str, str, float))
            # >>> method_name, types = DewarPositionerImpl.commands[command]
            # >>> method_name
            # 'setup'
            # >>> types
            # (str, str, float)
            try:
                method = getattr(self, method_name)
            except AttributeError, ex:
                success = False
                answer = "%s has no attribute %s" %(self, method_name)
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
                answer = '' if result is None else result
                success = True
            except TypeError, ex:
                success = False
                answer = ex.message
                logger.logError(answer)
                return (success, answer)
            except Exception, ex:
                success = False
                answer = ex.getReason() if hasattr(ex, 'getReason') else ex.message
                logger.logError(answer)
                return (success, answer)

        logger.logInfo('command %s executed' %cmd)
        return (success, answer)

        
class Control(object):
    def __init__(self):
        self.stop = False

