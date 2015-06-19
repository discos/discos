#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.

from Acspy.Clients.SimpleClient import PySimpleClient
import time
import math
import datetime
from Acspy.Common.TimeHelper import getTimeStamp

client = PySimpleClient()
antenna = client.getComponent("ANTENNA/Boss")

az_obj = antenna._get_rawAzimuth()
el_obj = antenna._get_rawElevation()

az_list = []
el_list = []
time_list = []
while True:
    try:
        az, compl = az_obj.get_sync()
        el, compl = el_obj.get_sync()
        az_list.append(math.degrees(az))
        el_list.append(math.degrees(el))
        time_list.append(getTimeStamp().value)
        time.sleep(0.05)
    except KeyboardInterrupt, e:
        break

outfile = open('outfile.data', 'w')
for i in range(len(az_list)):
    outfile.write('%u %s %s\n' %(time_list[i], az_list[i], el_list[i]))
outfile.close()
