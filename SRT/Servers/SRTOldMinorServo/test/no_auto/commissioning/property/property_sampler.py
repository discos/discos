#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.
"""Property sampler. Example of usage: `python property_sampler.py SRP torquePerc`"""

import unittest
import threading
import os
import time
from random import randrange as rr
from socket import *       
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common import TimeHelper
from maciErrTypeImpl import CannotGetComponentExImpl
import datetime
import sys

class NackEx(Exception):
    pass

def isAlmostEqual(seq1, seq2, delta = 0.2):
    try:
        if len(seq1) != len(seq2):
            return False
        else:
            for a, b in zip(seq1, seq2):
                if abs(a-b) > delta:
                    return False
    except TypeError:
        return abs(seq1 - seq2) < delta
    return True

if __name__ == "__main__":
    comp_name = sys.argv[1]
    axes_dict = {'SRP': 2, 'PFP': 0}
    axis = axes_dict[comp_name]
    axes_servo_dict = {'SRP': 2, 'PFP': slice(0,2)}
    axis_servo = axes_servo_dict[comp_name]

    servo_name = 'MINORSERVO/' + comp_name # SRP, GFR, M3R, PFP
    property_name = sys.argv[2] # torquePerc, engCurrent, engVoltage, ecc.
    pyclient = PySimpleClient()
    component = pyclient.getComponent(servo_name)
    print "Performing a setup..."
    component.setup(0)
    while not component.isReady():
        time.sleep(2)

    time.sleep(2)
    print "Setup done!"
    actPos_obj = component._get_actPos()
    actPos, cmp = actPos_obj.get_sync()
    max_pos = []
    min_pos = []
    for idx, pos in enumerate(actPos):
        if idx == axis:
            max_pos.append(component.getMaxPositions()[idx] - 1) # 1mm of margin
            min_pos.append(component.getMinPositions()[idx] + 1) # 1mm of margin
        else:
            max_pos.append(actPos[idx]) 
            min_pos.append(actPos[idx])

    component.setPosition(max_pos, 0)
    print '\nsetPosition(%s, %s)' %(max_pos, 0)

    print "Going to the target position..."
    while not isAlmostEqual(actPos[axis], max_pos[axis]):
        time.sleep(2)
        print "actPos: ", actPos[axis]
        actPos, cmp = actPos_obj.get_sync()
        
    print "\nMaximum position reached\n"

    text = ''
    while text != 'GO':
        text = raw_input('Enter GO  to go to the minimum position: ')

    print "Going to the minimum position: %s" %min_pos

    property_fun = getattr(component, '_get_' + property_name)
    property_obj = property_fun()
    value, cmp = property_obj.get_sync()
    component.setPosition(min_pos, TimeHelper.getTimeStamp().value + 20000000) # In 2 seconds

    data = []
    while not isAlmostEqual(actPos[axis], min_pos[axis]):
        actPos, cmp = actPos_obj.get_sync()
        property_value, cmp = property_obj.get_sync()
        time_stamp = TimeHelper.getTimeStamp().value
        data.append((time_stamp, actPos, property_value))
        time.sleep(0.05)

    print "\nMinimum position reached\n"

    out_file = open('../data/property_sampler.data', 'w')
    starting_time = data[0][0]
    old_time = TimeHelper.getTimeStamp().value
    for t, pos, value in data:
        time_ = (t - starting_time) / (10**4)
        out_file.write("\n#" + "-"*68)
        out_file.write("\nTime: exe_time + %0*d ms" %(3, time_ - old_time))
        out_file.write("\nPosition: %s" %pos[axis])
        out_file.write("\nProperty(%s): %s" %(property_name, value[axis_servo]))
        old_time = time_

    torque_slave = []
    torque_master = []
    for item in data:
        torque = data[2]
        torque_slave.append(torque[0])
        torque_master.append(torque[1])

    print "\nMaximum slave torque going to the minimum position: %s" %max(torque_slave)
    print "\nMaximum master torque going to the minimum position: %s" %max(torque_master)

    text = ''
    while text != 'GO':
        text = raw_input('Enter GO  to go back: ')

    # Go Back
    out_file.write("\n\n# ----- GO BACK -----\n")
    component.setPosition(max_pos, TimeHelper.getTimeStamp().value + 20000000) # In 2 seconds

    data = []
    while not isAlmostEqual(actPos[axis], max_pos[axis]):
        actPos, cmp = actPos_obj.get_sync()
        property_value, cmp = property_obj.get_sync()
        time_stamp = TimeHelper.getTimeStamp().value
        data.append((time_stamp, actPos, property_value))
        time.sleep(0.05)

    print "\nMax position reached\n"
    print "Writing to the file..."

    for t, pos, value in data:
        out_file.write("\n#" + "-"*68)
        out_file.write("\nTime: exe_time + %0*d ms" %(3, (t - starting_time) / (10**4)))
        out_file.write("\nPosition: %s" %pos[axis])
        out_file.write("\nProperty(%s): %s" %(property_name, value[axis_servo]))

    torque_slave = []
    torque_master = []
    for item in data:
        torque = data[2]
        torque_slave.append(torque[0])
        torque_master.append(torque[1])

    print "\nMaximum slave torque going to the minimum position: %s" %max(torque_slave)
    print "\nMaximum master torque going to the minimum position: %s" %max(torque_master)

    out_file.close()
    print "DONE!"
    pyclient.releaseComponent(servo_name)

