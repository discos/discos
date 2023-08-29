# Marco Buttu <marco.buttu@inaf.it> | Jul 2023
import time
import concurrent.futures
from datetime import datetime as dt
from math import sqrt, degrees
from threading import Thread
import ephem

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
        self.stop_threads = False
        for thread in (self.sunCheck, self.windCheck):
            t = Thread(target=thread, daemon=True)
            t.start()

    def initialize(self):
        # addProperty(self, 'status', devio_ref=StatusDevIO())
        pass

    def cleanUp(self):
        self.stop_threads = True
        # self.supplier.disconnect()

    def setup(self, code):
        self.log(f"Supervisor.setup({code})", Management.C_INFO)
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
                self.log(
                    f"Configuration '{code}' not found",
                    Management.C_ERROR,
                )

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
            self.log(msg, Management.C_ERROR)
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

    # TODO: fare un metodo/funzione che ottiene il component, passandole
    # il nome del component. Questo serve per il logging e per ogni volta
    # che ci occorre un client

    def windCheck(self):
        while True:
            if self.stop_threads:
                print("Stopping windCheck")
                break
            time.sleep(10)
            print("WIND: windAvoidance: ", self.windAvoidance)
        print("windCheck() stopped")

    def disconnect(self, client):
        try:
            if client.isLoggedIn():
                client.disconnect()
        except Exception as ex:
            self.log(
                "Cannot disconnect the client",
                Management.C_WARNING,
            )

    def log(self, message, level=Management.C_WARNING):
        try:
            print(message)
            logger_name = "MANAGEMENT/CustomLogger"
            client = PySimpleClient()
            logger = client.getComponent(logger_name)
            logger.emitLog(message, level)
        except CannotGetComponentEx as ex:
            print(f"Cannot get {logger_name}")
        except Exception as ex:
            print(f"Unexpected exception in log(): {ex}")
        finally:
            self.disconnect(client)

    def sunCheck(self):
        print("sunCheck() thread started")
        antenna_name = "ANTENNA/Boss"
        mount_name = "ANTENNA/Mount"
        observatory_name = "ANTENNA/Observatory"
        lat, lon = None, None
        obs = ephem.Observer()
        sun = ephem.Sun()
        stop_notified = False
        while True:
            time.sleep(1)
            if self.stop_threads:
                print("Stopping sunCheck() thread")
                break
            if self.sunAvoidance == "OFF":
                continue  # Sleep again

            client = PySimpleClient()
            if not (lat and lon):
                try:
                    observatory = client.getComponent(observatory_name)
                    lat, compl = observatory.latitude.get_sync()
                    lon, compl = observatory.longitude.get_sync()
                except CannotGetComponentEx as ex:
                    self.log(f"Supervisor can not get {observatory_name}")
                except Exception as ex:
                    print(f"Unexpected exception: {ex}")
                finally:
                    self.disconnect(client)
                    continue  # Sleep again

            try:
                antenna = client.getComponent(antenna_name)
            except CannotGetComponentEx as ex:
                self.log(
                    f"Can not get component {antenna_name}",
                    Management.C_ERROR,
                )
                self.disconnect(client)
                continue  # Sleep again

            t = getTimeStamp().value
            try:
                az, compl = antenna.observedAzimuth.get_sync()
                el, compl = antenna.observedElevation.get_sync()
                az, el = degrees(az), degrees(el)
            except Exception as ex:
                self.log(
                    f"Cannot get antenna coordinates: {ex}",
                    Management.C_ERROR,
                )
                self.disconnect(client)
                continue  # Sleep again
    
            obs.lat, obs.lon, obs.date = str(lat), str(lon), dt.utcnow()
            sun.compute(obs)
            sun_az, sun_el = degrees(sun.az), degrees(sun.alt)

            if (diff := abs(sun_az - az)) <= 180:
                delta_az = diff
            else:
                delta_az = 360 - diff

            distance = sqrt(delta_az**2 + (sun_el - el)**2)
            if distance < self.sunLimit:
                try:
                    mount = client.getComponent(mount_name)
                    az_mode, compl = mount.azimuthMode.get_sync()
                    el_mode, compl = mount.elevationMode.get_sync()
                    if str(az_mode) == str(el_mode) == "ACU_STOP":
                        if not stop_notified:
                            self.log(
                                f"Antenna stopped, pointing close to the Sun",
                                Management.C_WARNING,
                            )
                            stop_notified = True
                        continue
                except CannotGetComponentEx as ex:
                    self.log(
                        f"Can not get component {mount_name}",
                        Management.C_WARNING,
                    )
                except Exception:
                    pass

                self.log(f"Supervisor: stopping antenna, pointing close to the Sun")
                self.log(f"Sun(az, el) -> ({sun_az:.2f}, {sun_el:.2f})", Management.C_INFO)
                self.log(f"Ant(az, el) -> ({az:.2f}, {el:.2f})", Management.C_INFO)
                self.log(f"Safe Limit -> {self.sunLimit:.2f}", Management.C_INFO)
                self.log(f"Distance from Sun -> {distance:.2f}", Management.C_INFO)
                try:
                    antenna.stop()
                except Exception as ex:
                    self.log(f"Cannot stop the antenna: {ex}")
                    self.disconnect(client)
                    continue  # Sleep again

            if stop_notified:
                self.log(f"Antenna is now pointing at safe distance from the Sun")
            stop_notified = False
            self.disconnect(client)
        print("sunCheck() thread stopped")


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
            self.log(answer, Management.C_ERROR)
            return (success, answer)

        if command not in SupervisorImpl.commands:
            answer = f"Error - command '{command}' does not exist"
            self.log(answer, Management.C_ERROR)
            return (False, answer)
        else:
            method_name, types = SupervisorImpl.commands[command]
            if len(types) != len(args):
                answer = (
                    f"Error - expecting {len(types)} arguments, {len(args)} given\n" 
                    f"type help({command}) for details"
                )
                self.log(answer, Management.C_ERROR)
                return (False, answer)

            try:
                method = getattr(self, method_name)
            except AttributeError as ex:
                answer = f"Error - Supervisor.{method_name}() does not exist"
                self.log(answer, Management.C_ERROR)
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
                self.log(f"{ex.message}\n{answer}", Management.C_ERROR)
                return (False, answer)
            except ComponentErrors.ComponentErrorsEx as ex:
                answer = f"Error - {ex.errorTrace.data}"
                self.log(answer, Management.C_ERROR)
                return (False, answer)
            except Exception as ex:
                answer = "Error - {unexpected exception}"
                self.log(answer, Management.C_ERROR)
                return (False, answer)

        self.log(f"command '{command}' executed", Management.C_INFO)
        return (success, answer)
