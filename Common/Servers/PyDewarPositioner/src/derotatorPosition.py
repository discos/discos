#!/usr/bin/env python

from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.TimeHelper import getTimeStamp
from DewarPositioner.posgenerator import PosGenerator
import time

client = PySimpleClient()
latitude = 0.68928288149012062

dp = client.getComponent('RECEIVERS/DewarPositioner')
antenna = client.getComponent('ANTENNA/Boss')

counter = 0
while True:
    position = dp.getPosition()
    t = getTimeStamp().value + 1*10*6 # 100 ms in the future
    az, el = antenna.getRawCoordinates(t) # Values in radians
    parallactic = PosGenerator.getParallacticAngle(latitude, az, el)
    print "%d -> %.2f  -- parallactic: %.2f" %(counter, position, parallactic)
    counter += 1
    time.sleep(3)
