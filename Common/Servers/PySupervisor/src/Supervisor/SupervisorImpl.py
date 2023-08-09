# Marco Buttu <marco.buttu@inaf.it> | Jul 2023
import time
from datetime import datetime as dt
from math import radians
from multiprocessing import Queue
from Antenna__POA import Supervisor as POA
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as cc
from Acspy.Servants.ContainerServices import ContainerServices as services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as lcycle
from Acspy.Util.BaciHelper import addProperty
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Nc.Supplier import Supplier
from Acspy.Common.TimeHelper import getTimeStamp
from maciErrType import CannotGetComponentEx
from ACSErrTypeCommonImpl import CORBAProblemExImpl
import cdbErrType
from Acspy.Util import ACSCorba

import Management
import ComponentErrorsImpl
import ComponentErrors

from IRAPy import logger

import ephem

class SupervisorEx(Exception):
    pass


class SupervisorImpl(POA, cc, services, lcycle):
 
    commands = {
        # command_name: (method_name, (type_of_arg1, ..., type_of_argN))
        'supervisorSetup': ('setup', (str,)), 
        'supervisorGetSetup': ('getSetup', ()), 
        'supervisorSetWindAvoidance': ('setWindAvoidance', (str,)), 
        'supervisorGetWindAvoidance': ('getWindAvoidance', ()), 
        'supervisorSetWindSafeLimit': ('setWindSafeLimit', (float,)), 
        'supervisorGetWindSafeLimit': ('getWindSafeLimit', ()), 
        'supervisorSetSunAvoidance': ('setSunAvoidance', (str,)), 
        'supervisorGetSunAvoidance': ('getSunAvoidance', ()), 
        'supervisorSetSunSafeLimit': ('setSunSafeLimit', (float,)), 
        'supervisorGetSunSafeLimit': ('getSunSafeLimit', ()), 
        'supervisorGetSunOffset': ('getSunOffset', ()),
        'supervisorIsTracking': ('isTracking', ()), 
    }


    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self.dao_path = 'alma/ANTENNA/Supervisor'
        self.setup("default")
        # TODO: the publisher?

    def initialize(self):
        # addProperty(self, 'status', devio_ref=StatusDevIO())
        pass

    def cleanUp(self):
        # self.supplier.disconnect()
        pass

    def setup(self, code):
        logger.logNotice(f"Supervisor.setup({code})")
        self.receiverSetup = "unknown"
        self.windAvoidance = "ON"
        self.sunAvoidance = "ON"
        self.sunOffset = 0
        dao = ACSCorba.cdb().get_DAO_Servant(self.dao_path)
        self.windLimit = float(dao.get_field_data("WindLimit"))
        code = code.upper()
        try:
            self.sunLimit = float(dao.get_field_data(f"SunLimit{code}"))
            self.receiverSetup = code
        except cdbErrType.CDBFieldDoesNotExistEx:
            self.sunLimit = float(dao.get_field_data("SunLimitDEFAULT"))
            self.receiverSetup = "DEFAULT"
            if code != "DEFAULT":
                logger.logWarning(f"Configuration '{code}' not found")

    def getSetup(self):
        return self.receiverSetup

    def setWindAvoidance(self, mode):
        self.setAvoidance('wind', mode)

    def setSunAvoidance(self, mode):
        self.setAvoidance('sun', mode)

    def setAvoidance(self, target, mode):
        allowed_modes = ("ON", "OFF")
        if mode.upper() in allowed_modes:
            setattr(self, f"{target}Avoidance", mode)
        else:
            msg = f"mode '{mode}' not in {allowed_modes}"
            logger.logError(msg)
            exc = ComponentErrorsImpl.NotAllowedExImpl(msg)
            raise exc.getComponentErrorsEx()

    def getWindAvoidance(self):
        return self.windAvoidance

    def getSunAvoidance(self):
        return self.sunAvoidance
    
    def setWindSafeLimit(self, limit):
        self.windLimit = limit

    def getWindSafeLimit(self):
        return self.windLimit
 
    def setSunSafeLimit(self, limit):
        self.sunLimit = limit

    def getSunSafeLimit(self):
        return self.sunLimit

    def getSunOffset(self):
        return self.sunOffset

    def isTracking(self):
        return True

    def command(self, cmd):
        """Execute the command `cmd`
        
        This method returns a tuple (success, answer), where success is a boolean
        which indicates whether the command has been executed correctly (success==True)
        or not (success = False). The returned `answer` is a string that represents the
        error message in case of error, the value returned from the method in
        case it returns a non Null object, an empty string in case it returns None.
        """
        try:
            command, args_str = cmd.split('=') if '=' in cmd else (cmd, '')
            command = command.strip()
            args = [item.strip() for item in args_str.split(',') if item.strip()]
        except ValueError:
            success = False 
            answer = "Error - invalid command: maybe there are too many symbols of ="
        except Exception as ex:
            success = False 
            answer = ex.message
        else:
            success = True

        if not success:
            logger.logError(answer)
            return (success, answer)

        if command not in SupervisorImpl.commands:
            answer = f"Error - command '{command}' does not exist"
            logger.logError(answer)
            return (False, answer)
        else:
            method_name, types = SupervisorImpl.commands[command]
            if len(types) != len(args):
                answer = (
                    f"Error - expecting {len(types)} arguments, {len(args)} given\n" 
                    f"type help({command}) for details"
                )
                logger.logError(answer)
                return (False, answer)

            try:
                method = getattr(self, method_name)
            except AttributeError as ex:
                answer = f"Error - Supervisor.{method_name}() does not exist"
                logger.logError(answer)
                return (False, answer)
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
                answer = f"Error - wrong syntax.\nType help({command}) for details"
                logger.logError(f"{ex.message}\n{answer}")
                return (False, answer)
            except ComponentErrors.ComponentErrorsEx as ex:
                answer = f"Error - {ex.errorTrace.data}"
                logger.logError(answer)
                return (False, answer)
            except Exception as ex:
                answer = "Error - {unexpected exception}"
                logger.logError(answer)
                return (False, answer)

        logger.logInfo(f"command '{command}' executed")
        return (success, answer)

def getSunAzEl():
    srt = ephem.Observer()
    sun = ephem.Sun()
    srt.lat, srt.lon, srt.date = '39.493156', '9.2451556', dt.utcnow()
    sun.compute(srt)
    return sun.az, sun.alt
