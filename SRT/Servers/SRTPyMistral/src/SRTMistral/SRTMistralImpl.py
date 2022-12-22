import time
import socket
from threading import Timer, Lock

import Management
import Backends
import BackendsErrors
from Backends__POA import SRTMistral as POA
from bulkdata__POA import BulkDataSender

import ComponentErrorsImpl
import ComponentErrors
import cdbErrType

from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as cc
from Acspy.Servants.ContainerServices import ContainerServices as services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as lcycle
from Acspy.Util import ACSCorba
from Acspy.Util.BaciHelper import addProperty
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.TimeHelper import getTimeStamp
from ACSErrTypeCommonImpl import CORBAProblemExImpl
from Acspy.Nc.Supplier import Supplier

from IRAPy import logger

from simulators.utils import ACS_TO_UNIX_TIME
from simulators.backend.grammar import (
    REQUEST,
    REPLY,
    TAIL,
    SEPARATOR,
    OK,
    FAIL,
    INVALID,
)

from SRTMistral.devios import NumberDevIO, StringDevIO

__copyright__ = "Marco Buttu <marco.buttu@inaf.it>"


class SRTMistralImpl(POA, cc, services, lcycle, BulkDataSender):
 
    commands = {
        # command_name: (method_name, (type_of_arg1, ..., type_of_argN))
        'mistralStatus': ('_status_str', ()), 
        'mistralSetup': ('setup', ()), 
        'mistralVnaSweep': ('vnaSweep', ()), 
        'mistralTargetSweep': ('targetSweep', ()), 
        'mistralStartAcquisition': ('_start_now', ()),
        'mistralStopAcquisition': ('sendStop', ()),
        'mistralSetFilename': ('setTargetFileName', (str,)),
        'mistralGetFilename': ('getTargetFileName', ()),
        # Testing commands
        'test': ('_test', (int, int)), 
        'test_no_method': ('no_method', ()), 
    }

    def __init__(self):
        services.__init__(self)
        cc.__init__(self)
        POA.__init__(self)
        BulkDataSender.__init__(self)
        self.protocol_version = 'unknown'
        self._startID = None
        self._status_thread = None
        self.status_data = StatusData()
        self.socket_lock = Lock()
        self.supplier = None

        # Get attributes from CDB
        attributes = (
            ('IPAddress', str),
            ('Port', int),
            ('SocketTimeout', float),
            ('StatusFrequency', float),
        )
        try:
            dal = ACSCorba.cdb()
            dao = dal.get_DAO_Servant('alma/BACKENDS/SRTMistral')
            for (name, type_) in attributes:
                value = type_(dao.get_field_data(name))
                setattr(self, name, value)
        except cdbErrType.CDBRecordDoesNotExistEx:
            raeson = f"CDB record {path} does not exists"
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except cdbErrType.CDBFieldDoesNotExistEx:
            raeson = f"CDB field {attribute} does not exist"
            logger.logWarning(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc

        try:
            self.supplier = Supplier(Backends.MISTRAL_DATA_CHANNEL)
        except CORBAProblemExImpl as ex:
            logger.logError('cannot create MISTRAL data channel')
            logger.logDebug(f'cannot create MISTRAL channel: {ex}')
        except Exception as ex:
            reason = ex.getReason() if hasattr(ex, 'getReason') else str(ex)
            logger.logError(reason)
        except:
            logger.logError('unexpected exception creating data channel')

        try:
            self._status_thread = services().getThread(
                    name='Publisher',
                    target=SRTMistralImpl.publisher,
                    args=(
                        self.update_status,
                        self.supplier,
                        self.status_data,
                        self.StatusFrequency,
                    )
            )
            self._status_thread.start()
        except AttributeError as ex:
            logger.logWarning('supplier not available')
            logger.logDebug('supplier not available: {ex}')
        except Exception as ex:
            logger.logError('cannot create status thread: {ex}')

    def __del__(self):
        try:
            self.cleanUp()
        except:
            pass

    def initialize(self):
        addProperty(self, 'time', devio_ref=NumberDevIO('time'))
        addProperty(self, 'inputsNumber', devio_ref=NumberDevIO('inputsNumber'))
        addProperty(self, 'sectionsNumber', devio_ref=NumberDevIO('sectionsNumber'))
        addProperty(self, 'integration', devio_ref=NumberDevIO('integration'))
        addProperty(self, 'backendName', devio_ref=StringDevIO('backendName'))
        addProperty(self, 'bandWidth', devio_ref=NumberDevIO('bandWidth'))
        addProperty(self, 'frequency', devio_ref=NumberDevIO('frequency'))
        addProperty(self, 'sampleRate', devio_ref=NumberDevIO('sampleRate'))
        addProperty(self, 'attenuation', devio_ref=NumberDevIO('attenuation'))
        addProperty(self, 'polarization', devio_ref=NumberDevIO('polarization'))
        addProperty(self, 'bins', devio_ref=NumberDevIO('bins'))
        addProperty(self, 'feed', devio_ref=NumberDevIO('feed'))
        addProperty(self, 'systemTemperature', devio_ref=NumberDevIO('systemTemperature'))
        addProperty(self, 'inputSection', devio_ref=NumberDevIO('inputSection'))

    def cleanUp(self):
        if self._startID:
            self._startID.cancel()
            self._startID.join()
        self.status_data.stop_thread = True
        time.sleep(self.StatusFrequency*2)
        if self._status_thread and self._status_thread.is_alive():
            self._status_thread.join()

    def request(self, cmd, *args):
        parameters = ','.join(str(i) for i in args)
        cmd = cmd if not parameters else f'{cmd},{parameters}'
        req = bytes(f'{REQUEST}{cmd}{TAIL}', encoding='ascii')
        try:
            if not self.socket_lock.acquire(timeout=2):
                raise TimeoutError('cannot acquire socket lock')
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(self.SocketTimeout)
            sock.connect((self.IPAddress, self.Port))
            # Look for the response tail (grammar.TAIL)
            stream = response = b''
            for i in range(256):  # Maximum number of bytes to discard
                stream += sock.recv(1)
                # Look for the response header (grammar.REPLY)
                if stream.endswith(REPLY.encode('ascii')):
                    response = b''
                    continue
                if response and stream.endswith(TAIL.encode('ascii')):
                    if response.startswith(b'version'):
                        sock.sendall(req)
                        response = b''
                        continue
                    else:
                        break
                if REPLY.encode('ascii') in stream:
                    response += stream[-1:]
            else:
                raise ValueError(f'header or tail not found in {stream}')
            return response.decode() 
        except ConnectionRefusedError:
            raise ConnectionRefusedError(
                f'connection refused from MISTRAL server'
            )
        except BrokenPipeError:
            raise BrokenPipeError(
                f"broken pipe while processing '{cmd}' command"
            )
        finally:
            self.socket_lock.release()
            sock.close()
 
    def _run(self, command_name, *args):
        try:
            if command_name == 'status':
                return self.status_data.status_str

            response = self.request(command_name, *args)
            cmd, code, *other = response.split(SEPARATOR)
            base = f'{cmd.upper()}: {code.upper()}'
            return base + f' -- {other[0].rstrip()}' if len(other) == 1 else base
        except Exception as ex:
            msg = str(ex)
            self.status_data.error_message = msg
            logger.logError(msg)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', msg)
            raise exc.getComponentErrorsEx()

    def update_status(self):
        try:
            response = self.request('status')
            cmd, code, timestamp, message, flag = response.split(SEPARATOR)
            result = f'{cmd.upper()}: {code.upper()} -- {message}'
            self.status_data.timeMark = timestamp
            
            if 'system not initialized' in message:
                self.status_data.ready = False
            else:
                self.status_data.ready = True

            if 'setup in progress' in message:
                self.status_data.initializing = True
            else:
                self.status_data.initializing = False
            
            if 'acquisition in progress' in message and int(flag):
                self.status_data.acquiring = True
            else:
                self.status_data.acquiring = False
                result += ' -- not acquiring'

            if 'sweep in progress' in message:
                self.status_data.sweeping = True
            else:
                self.status_data.sweeping = False

            if 'ready to run a task' in message:
                self.status_data.nop = True
            else:
                self.status_data.nop = False

            if code == FAIL:
                self.status_data.error_message = message
            else:
                self.status_data.error_message = ''

            self.status_data.status_str = result
            return self.status_data
        except Exception as ex:
            self.status_data.status_str = f'STATUS: FAIL -- {str(ex)}'
            self.status_data.error_message = str(ex)

    def _status_str(self):
        return self.status_data.status_str

    def setup(self):
        return self._run('setup')

    def vnaSweep(self):
        return self._run('vna-sweep')
 
    def targetSweep(self):
        return self._run('target-sweep')

    def _start_now(self):
        return self._run('start')

    def _start_at(self, t):
        if self._startID:
            self._startID.cancel()
        start_in = t - time.time()
        self._startID = Timer(start_in, self._start_now)
        self._startID.start()

    def sendData(self, stime):
        try:
            t = stime / ACS_TO_UNIX_TIME  # starting time
            if t == 0 or t <= time.time():
                return self._start_now()
            else:
                self._start_at(t)
        except Exception as ex:
            logger.logError(ex)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', str(ex))
            raise exc.getComponentErrorsEx()

    def sendStop(self):
        if self._startID:
            self._startID.cancel()
        return self._run('stop')

    def setTargetFileName(self, name):
        return self._run(f'set-filename,{name.strip()}')

    def getTargetFileName(self):
        return self._run(f'get-filename')

    def command(self, cmd):
        """Execute the command cmd
        
        This method returns a tuple (success, answer), where `success` is a boolean
        indicating wheather the command is executed correctly (success = True) or not
        (success = False).  The `answer` is a string representing the error message
        in case of error, the value returned from the method in case it returns a
        not Null object, an empty string in case it returns None.
        """
        try:
            command, args_str = cmd.split('=') if '=' in cmd else (cmd, '')
            command = command.strip()
            args = [item.strip() for item in args_str.split(',')]
        except ValueError:
            success = False 
            answer = 'ERROR: invalid command, maybe there are two symbols of ='
        except Exception as ex:
            success = False 
            answer = str(ex)
        else:
            success = True

        if not success:
            logger.logError(answer)
            return (success, answer)

        if command not in SRTMistralImpl.commands:
            success = False
            answer = f'ERROR: command `{command}` does not exist'
            logger.logError(answer)
            return (success, answer)
        else:
            # For instance:
            # >>> SRTMistralImpl.commands['foo']
            # ('setup', (str, str, float))
            # >>> method_name, types = SRTMistralImpl.commands[command]
            # >>> method_name
            # 'setup'
            # >>> types
            # (str, str, float)
            method_name, types = SRTMistralImpl.commands[command]
            # If we expect some arguments but there is not
            if types and not any(args):
                success = False
                answer = f'ERROR: missing arguments, type help({command}) for details'
                logger.logError(answer)
                return (success, answer)

            try:
                method = getattr(self, method_name)
            except AttributeError:
                success = False
                answer = f'ERROR: {self} has no attribute {method_name}'
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
                answer = f'ERROR: wrong arguments, type help({command}) for details'
                logger.logError(f'{ex}\n{answer}')
                return (success, answer)
            except ComponentErrors.ComponentErrorsEx as ex:
                success = False
                data_list = ex.errorTrace.data # A list
                reason = data_list[0].value if data_list else 'component error'
                answer = f'ERROR: {reason}'
                logger.logError(answer)
                return (success, answer)
            except Exception as ex:
                success = False
                answer = f'ERROR: {ex.getReason()}' if hasattr(ex, 'getReason') else ex
                logger.logError(answer)
                return (success, answer)

        logger.logInfo(f'command {cmd} executed')
        return (success, answer)

    @staticmethod
    def publisher(update_status, supplier, status_data, StatusFrequency):
        error = False
        while not status_data.stop_thread:
            try:
                data = update_status()
                if supplier:
                    event = Backends.MistralDataBlock(
                        getTimeStamp().value,
                        data.initializing,
                        data.ready,
                        data.acquiring,
                        data.sweeping,
                        data.nop,
                        data.error_message,
                        data.mngStatus,
                    )
                    supplier.publishEvent(simple_data=event)
                error = False
            except Exception as ex:
                reason = ex.getReason() if hasattr(ex, 'getReason') else str(ex)
                if not error:
                    logger.logError('cannot publish the status: {reason}')
                    error = True
            finally:
                time.sleep(StatusFrequency)

    def _test(self, x, y):
        pass


class StatusData:
    def __init__(self):
        self.stop_thread = False
        self.timeMark = 0
        self.initializing = False
        self.ready = False
        self.acquiring = False
        self.sweeping = False
        self.nop = False
        self.error_message = ""
        self.mngStatus = Management.MNG_OK
        self.status_str = ''
