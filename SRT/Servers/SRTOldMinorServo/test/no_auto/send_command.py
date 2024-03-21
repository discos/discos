#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.

from socket import *       

address = ('192.168.200.16', 10000)
# address = ('127.0.0.1', 10000)

# Direct connection used to match the property values
sockobj = socket(AF_INET, SOCK_STREAM)
sockobj.settimeout(1.0)
sockobj.connect(address)

servos = {
    'PFP': 0, # Prime Focus Positioner
    'SRP': 1, # SubReflector Positioner
    'GFR': 2, # Gregorian Feed Rotator
    'M3R': 3  # Mirror 3 Rotator
}

sockobj.sendall('#setsdatbitb16:2=0,0,0,15,1\r\n')
data = ''
while(True):
    data += sockobj.recv(1)
    if '\r\n' in data:
        print 'r> %r' %data
        break
