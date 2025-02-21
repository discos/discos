import time
from math import degrees
from datetime import datetime as dt

import ephem

import Antenna
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.TimeHelper import getTimeStamp

client = PySimpleClient()

def disconnect():
    if client.isLoggedIn():
        client.disconnect()

print("Getting mount, boss and supervisor components...")
mount = client.getComponent('ANTENNA/Mount')
boss = client.getComponent('ANTENNA/Boss')
supervisor = client.getComponent('ANTENNA/Supervisor')
print("Got component!")


def getAntennaCoordinates():
    az = boss.observedAzimuth.get_sync()
    el = boss.observedElevation.get_sync()
    az, compl = boss.observedAzimuth.get_sync()
    el, compl = boss.observedElevation.get_sync()
    return degrees(az), degrees(el)

def getAntennaMode():
    az_mode = mount.azimuthMode.get_sync()[0]
    el_mode = mount.elevationMode.get_sync()[0]
    return az_mode, el_mode

def goToSun():
    obs = ephem.Observer()
    sun = ephem.Sun()
    obs.lat, obs.lon, obs.date = "39.4930", "9.2451", dt.utcnow()
    sun.compute(obs)
    sun_az, sun_el = degrees(sun.az), degrees(sun.alt)
    mount.changeMode(Antenna.ACU_PRESET, Antenna.ACU_PRESET)
    mount.preset(sun_az, sun_el)

def getSunPosition():
    obs = ephem.Observer()
    sun = ephem.Sun()
    obs.lat, obs.lon, obs.date = "39.4930", "9.2451", dt.utcnow()
    sun.compute(obs)
    return degrees(sun.az), degrees(sun.alt)

def antennaSetup(code="CCB"):
    boss.setup(code)
    print(f"Waiting for AntennaBoss.setup({code})")
    time.sleep(8)
    mount.changeMode(Antenna.ACU_PRESET, Antenna.ACU_PRESET)
    mount.preset(0, 88)
    print("Moving to (0, 88)")

def setWindLimit(value):
    supervisor.setWindSafeLimit(value)

