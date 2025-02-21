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
        'supervisorGetSunDistance': ('getSunDistance', ()),
        'supervisorGetWind': ('getAverageWind', ()),
    }

    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self.dao_path = 'alma/ANTENNA/Supervisor'
        self.setup("default")
        self.stop_threads = False
        self.sunClient = self.windClient = None
        self.loadSunClient()
        self.loadWindClient()
        for thread in (self.sunCheck, self.windCheck):
            t = Thread(target=thread, daemon=True)
            t.start()

    def cleanUp(self):
        self.stop_threads = True

    def setup(self, code):
        self.log(f"Supervisor.setup({code})", Management.C_INFO)
        self.receiverSetup = "unknown"
        self.windAvoidance = "ON"
        self.sunAvoidance = "ON"
        self.sunDistance = 180
        self.wind = 0
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

    def getSunDistance(self):
        return self.sunDistance

    def getAverageWind(self):
        return self.wind

    def loadWindClient(self):
        try:
            if self.windClient and self.windClient.isLoggedIn():
                pass
            else:
                self.disconnectWindClient()
                self.windClient = PySimpleClient()
        except Exception as ex:
            self.log(
                f"loadWindClient: {ex}",
                Management.C_ERROR,
            )

    def loadSunClient(self):
        try:
            if self.sunClient and self.sunClient.isLoggedIn():
                pass
            else:
                self.disconnectSunClient()
                self.sunClient = PySimpleClient()
        except Exception as ex:
            self.log(
                f"loadSunClient: {ex}",
                Management.C_ERROR,
            )

    def disconnectSunClient(self):
        try:
            self.sunClient.disconnect()
        except Exception as ex:
            self.log(
                "Cannot disconnect sunClient",
                Management.C_WARNING,
            )

    def disconnectWindClient(self):
        try:
            self.windClient.disconnect()
        except Exception as ex:
            self.log(
                "Cannot disconnect windClient",
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
            try:
                client.disconnect()
            except Exception:
                print(f"Error disconnecting the log client")

    def windCheck(self):
        print("windCheck() thread started")
        stow_notified = False
        mount_name = "ANTENNA/Mount"
        scheduler_name = "IDL:alma/Management/Scheduler:1.0"
        weather_name = "WEATHERSTATION/WeatherStation"
        antenna_name = "ANTENNA/Boss"
        wind_values = []
        while True:
            try:
                for i in range(5):  # Sleep for 5 seconds, check stop signal
                    if self.stop_threads:
                        break
                    else:
                        time.sleep(1)

                if self.stop_threads:
                    print("Stopping windCheck() thread")
                    break

                if self.windAvoidance == "OFF":
                    continue  # Sleep again

                self.loadWindClient()
                try:
                    el_mode = ""
                    el = 0
                    mount = self.windClient.getComponent(mount_name)
                    el_mode = str(mount.elevationMode.get_sync()[0])
                    el = mount.elevation.get_sync()[0]
                except CannotGetComponentEx as ex:
                    self.log(
                        f"Can not get component {mount_name}",
                        Management.C_WARNING,
                    )
                except Exception as ex:
                    self.log(f"Can not get component {mount_name}: {ex}")

                try:
                    ws = self.windClient.getComponent(weather_name)
                    value = ws.getWindSpeedAverage()
                except CannotGetComponentEx as ex:
                    self.log(
                        f"Can not get component {weather_name}",
                        Management.C_ERROR,
                    )
                except Exception as ex:
                    self.log(f"Unexpected exception: {ex}")
                    continue  # Sleep again

                if len(wind_values) < 30:
                    wind_values.append(value)
                else:
                    wind_values.pop()
                    wind_values.insert(0, value)

                self.wind = sum(wind_values)/len(wind_values) if wind_values else 0
                if self.wind >= self.windLimit:
                    if el_mode == "ACU_STOW":
                        if not stow_notified:
                            self.log(
                                f"Parking antenna, too much wind",
                                Management.C_WARNING,
                            )
                            stow_notified = True
                        continue

                    try:
                        scheduler = self.windClient.getDefaultComponent(scheduler_name)
                        scheduler.stopSchedule()
                    except CannotGetComponentEx as ex:
                        self.log(
                            f"Supervisor can not get {scheduler_name}",
                            Management.C_WARNING,
                        )
                    except Exception as ex:
                        self.log(f"Can not get {scheduler_name}: {ex}")

                    try:
                        antenna = self.windClient.getComponent(antenna_name)
                    except CannotGetComponentEx as ex:
                        self.log(
                            f"Can not get component {antenna_name}",
                            Management.C_ERROR,
                        )
                    except Exception as ex:
                        self.log(f"Can not get {antenna_name}: {ex}")
                        continue  # Sleep again

                    if el_mode == "ACU_STANDBY" and abs(90-el) < 0.5:
                        continue  # Antenna parked

                    try:
                        self.log("Too much wind", Management.C_WARNING)
                        self.log(f"Safe limit -> {self.windLimit:.2f}", Management.C_INFO)
                        self.log(f"Average wind -> {self.wind:.2f}", Management.C_INFO)
                        self.log(f"Supervisor: parking antenna, windy")
                        antenna.stop()
                        antenna.park()  # Finally park the antenna
                        self.log("Parking antenna", Management.C_INFO)
                        stow_notified = True
                    except Exception as ex:
                        self.log(f"Can not park the antenna: {ex}")
                        continue  # Sleep again

                stow_notified = False
            except Exception as ex:
                self.log(f"Unexpected exception in windCheck(): {ex}")

        print("sunCheck() thread stopped")


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
            try:
                time.sleep(1)
                if self.stop_threads:
                    print("Stopping sunCheck() thread")
                    break
                if self.sunAvoidance == "OFF":
                    continue  # Sleep again

                self.loadSunClient()
                if not (lat and lon):
                    try:
                        observatory = self.sunClient.getComponent(observatory_name)
                        lat, compl = observatory.latitude.get_sync()
                        lon, compl = observatory.longitude.get_sync()
                    except CannotGetComponentEx as ex:
                        self.log(f"Supervisor can not get {observatory_name}")
                    except Exception as ex:
                        self.log(f"Can not get component {observatory_name}: {ex}")
                    finally:
                        continue  # Sleep again

                try:
                    antenna = self.sunClient.getComponent(antenna_name)
                except CannotGetComponentEx as ex:
                    self.log(
                        f"Can not get component {antenna_name}",
                        Management.C_ERROR,
                    )
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
                    continue  # Sleep again

                limit = self.sunLimit
                az_mode = el_mode = ""
                try:
                    mount = self.sunClient.getComponent(mount_name)
                    az_mode = str(mount.azimuthMode.get_sync()[0])
                    el_mode = str(mount.elevationMode.get_sync()[0])
                except CannotGetComponentEx as ex:
                    self.log(
                        f"Can not get component {mount_name}",
                        Management.C_WARNING,
                    )
                except Exception as ex:
                    self.log(f"Can not get component {mount_name}: {ex}")
 
                obs.lat, obs.lon, obs.date = str(lat), str(lon), dt.utcnow()
                sun.compute(obs)
                sun_az, sun_el = degrees(sun.az), degrees(sun.alt)
                if (diff := abs(sun_az - az)) <= 180:
                    delta_az = diff
                else:
                    delta_az = 360 - diff

                self.sunDistance = sqrt(delta_az**2 + (sun_el - el)**2)
                if self.sunDistance < limit:
                    if az_mode == el_mode == "ACU_STOP":
                        time.sleep(5)
                        self.log(
                            f"Distance from Sun -> {self.sunDistance:.2f}",
                            Management.C_INFO
                        )
                        self.log(
                            f"Sun safe limit -> {limit:.2f}",
                            Management.C_INFO
                        )
                        if not stop_notified:
                            self.log(
                                "Antenna stopped, pointing close to the Sun",
                                Management.C_WARNING,
                            )
                            stop_notified = True
                        continue  # Sleep again

                    self.log(f"Supervisor: stopping antenna, pointing close to the Sun")
                    self.log(f"Sun(az, el) -> ({sun_az:.2f}, {sun_el:.2f})", Management.C_INFO)
                    self.log(f"Ant(az, el) -> ({az:.2f}, {el:.2f})", Management.C_INFO)
                    self.log(f"Safe Limit -> {limit:.2f}", Management.C_INFO)
                    self.log(f"Distance from Sun -> {self.sunDistance:.2f}", Management.C_INFO)
                    try:
                        antenna.stop()
                        self.log(
                            f"Antenna stopped, pointing close to the Sun",
                            Management.C_WARNING,
                        )
                        stop_notified = True
                    except Exception as ex:
                        self.log(f"Cannot stop the antenna: {ex}")
                    finally:
                        continue  # Sleep again

                stop_notified = False
            except Exception as ex:
                self.log(f"Unexpected exception in sunCheck(): {ex}")

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
