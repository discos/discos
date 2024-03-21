#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.

# TODO: this test does not stress the MSCU. We have to command a set of positions
#       and aftere, and when the SRP is moving, we have to read the status of the
#       SPR and of other servos

from __future__ import division
from socket import *       
import datetime
import sys

address = ('192.168.200.16', 10000)

# Direct connection used to match the property values
sockobj = socket(AF_INET, SOCK_STREAM)
#sockobj.settimeout(0.1)
sockobj.connect(address)

servos = {
    'PFP': 0, # Prime Focus Positioner
    'SRP': 1, # SubReflector Positioner
    'GFR': 2, # Gregorian Feed Rotator
    'M3R': 3  # Mirror 3 Rotator
}

headers = ('#', '!', '?', '@')
closers = ('\r\n', '\n\r', '\r', '\n')
cmd_num = 0
total_time = 2 # 2 seconds of test
starting_time = datetime.datetime.now()
actual_time = starting_time
timestamps = []

while((actual_time - starting_time).seconds < total_time):
    actual_time = datetime.datetime.now()
    # print
    cmd_num += 1
    command = '#getstatus:%d=%d,0' %(cmd_num, servos['SRP'])
    command += '\r\n' 

    found = False
    data = ""
    try:
        while(not found):
            # print "s> %r" %command # The requesst
            sockobj.sendall(command)
            while(True):
                data += sockobj.recv(1)
                if closers[0] in data:
                    # print 'r> %r' %data # The answer
                    cmd, par = data.split('>')
                    timestamp_str = par.split(',')[0]
                    timestamps.append(long(timestamp_str.strip()))
                    if ':%d=' %cmd_num in data:
                        found = True
                        if data.startswith('?') or data.startswith('!NAK'):
                            data = ""
                            break
                    data = ""
    except:
        print sys.exc_info()[1]
        pass


diffs = []
for idx, value in enumerate(timestamps):
    if idx > 0:
        diffs.append(value - timestamps[idx-1])

print "Max diff: ", max(diffs) / 10**4, "ms"
print "Min diff: ", min(diffs) / 10**4, "ms"
print "Average diff: ", (sum(diffs) / len(diffs)) / 10**4, "ms"

