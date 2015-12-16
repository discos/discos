# mscu-start
# acsStartContainer -cpp MinorServoContainer
# acsStartContainer -cpp MinorServoBossContainer
# acsStartContainer -py AntennaBossSimulatorContainer

from __future__ import with_statement

import threading
import time
import math
import datetime
import Antenna

from multiprocessing import Value
from random import randrange as rr
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.TimeHelper import getTimeStamp

antenna_client = PySimpleClient()

# Start a thread in order to set the AntennaBoss position
stop = Value('i', 0)
stop.value
antenna = antenna_client.getComponent('ANTENNA/Boss')
def set_position(antenna, stop):
    az0 = rr(-100, 50)
    el0 = rr(5, 30)
    while True:
        az = math.radians(az0 + 0.005)
        el = math.radians(el0 + 0.01)
        antenna.setOffsets(az, el, Antenna.ANT_HORIZONTAL)
        time.sleep(0.1)
        if stop.value:
            break

t = threading.Thread(target=set_position, args = (antenna, stop))
t.daemon = True
t.start()

# Get the MinorServo axes position
msclient = PySimpleClient()
boss = msclient.getComponent('MINORSERVO/Boss')
boss.setup('KKG')
while not boss.isReady():
    time.sleep(2)

starting_time = datetime.datetime.now()
total_time = 1800 # seconds
deltas = []
timestamps = []
while (datetime.datetime.now() - starting_time).seconds < total_time:
    # Random timevalue in the past 2 minutes
    target_time = getTimeStamp().value - rr(10**7, 200*10**7)
    t0 = datetime.datetime.now()
    axes = boss.getAxesPosition(target_time)
    delta = datetime.datetime.now() - t0
    sharp_delta = (delta.seconds + delta.microseconds)*1.0 / 10**6
    timestamps.append((t0 - starting_time).seconds)
    deltas.append(sharp_delta)
    time.sleep(1)

print 'Total test time: ', total_time
print 'max_time: %.4f' % max(deltas)
print 'min_time: %.4f' % min(deltas)
with open('outfile', 'w') as outfile:
    for timestamp, delta in zip(timestamps, deltas):
        outfile.write('%d %.4f\n' % (timestamp, delta))

# Stop the Antenna set_position thread
stop.value = True
t.join(5)
