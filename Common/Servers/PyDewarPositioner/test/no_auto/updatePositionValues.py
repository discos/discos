from __future__ import with_statement
import unittest
import random
import time
from maciErrType import CannotGetComponentEx
from DewarPositioner.positioner import Positioner, NotAllowedError, PositionerError
from DewarPositionerMockers.mock_components import MockDevice, MockSource
from DewarPositioner.cdbconf import CDBConf
from DewarPositioner.posgenerator import PosGenerator
from Acspy.Clients.SimpleClient import PySimpleClient

client = PySimpleClient()
cdbconf = CDBConf()
p = Positioner(cdbconf)
source = MockSource()
device = client.getComponent('RECEIVERS/SRTKBandDerotator')

# Get the site latitude
observatory = client.getComponent('ANTENNA/Observatory')
lat_obj = observatory._get_latitude()
latitude, compl = lat_obj.get_sync()
site_info = {'latitude': latitude}

cdbconf.setup('KKG')
cdbconf.setConfiguration('BSC')
time.sleep(0.3)

p.setup(site_info, source, device)
p.setRewindingMode('MANUAL')
time.sleep(1) # Wait a tit (the setup execute a thread)
posgen = PosGenerator()
gen = posgen.parallactic(source, site_info)
cmds = [] # Commanded positions
acts = [] # Actual positions
try:
    first_position = 74.52 # (az=175, el=85)
    device.setPosition(first_position) 
    print 'Going to %.2f...' %first_position
    # Wait until it reaches the first position
    while device.getActPosition() < (first_position - 1):
        print 'actual position: %.2f' %device.getActPosition()
        time.sleep(3)

    p.startUpdating('MNG_EQ_LAT', 'ANT_NORTH')
    for i in range(0, 20):
        source.setAzimuth(175 + i*0.25) # From 175 to 179.75
        source.setElevation(85 + i*0.25) # From 85 to 89.75
        time.sleep(0.5)
        cmd = device.getCmdPosition()
        act = device.getActPosition()
        cmds.append(cmd)
        acts.append(act)
        print 'cmd, act: %.2f  %.2f' %(cmd, act)
finally:
    p.stopUpdating()
    time.sleep(2)
    p.park()
    time.sleep(3)

max_diff = max([abs(a-c) for a, c in zip(acts, cmds)])
print 'maximum difference: %.2f' %max_diff

