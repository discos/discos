#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.

from Acspy.Clients.SimpleClient import PySimpleClient
import time
import math

client = PySimpleClient()
antenna = client.getComponent("ANTENNA/Boss")

az_obj = antenna._get_rawAzimuth()
el_obj = antenna._get_rawElevation()

az_list = []
el_list = []
while True:
    try:
        az, compl = az_obj.get_sync()
        el, compl = el_obj.get_sync()
        az_list.append(math.degrees(az))
        el_list.append(math.degrees(el))
        time.sleep(0.5)
        print math.degrees(az), math.degrees(el)
    except KeyboardInterrupt, e:
        break

outfile = open('outfile.data', 'w')
len_ = min(len(az_list), len(el_list))
for i in range(len_):
    outfile.write('%s %s\n' %(az_list[i], el_list[i]))
outfile.close()
