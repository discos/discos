#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.
"""Use this module to test the minor servo setPosition() method, with `exe_time != 0`.
   The positions are commanded all at once."""

import unittest
import threading
import os
import time
from random import randrange as rr
from socket import *       
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common import TimeHelper
from maciErrTypeImpl import CannotGetComponentExImpl
from parameters import *

server = servers['MSCU']

class NackEx(Exception):
    pass

class TestPositioning(unittest.TestCase):
    """Test the minor servo properties."""

    def setUp(self):
        """Get a reference to every minor servo component."""
        # Direct connection used to match the property values
        self.sockobj = socket(AF_INET, SOCK_STREAM)
        self.sockobj.settimeout(socket_timeout)
        # Connect to the MSCU
        self.sockobj.connect(server) 
        self.pyclient = PySimpleClient()
        self.cmd_num = 0
        self.component_name = "MINORSERVO/SRP"
        self.component = self.pyclient.getComponent(self.component_name)

    def _tearDownClass():
        print "in tearDown"
        self.sockobj.close()
        self.pyclient.releaseComponent(self.component_name)

    def test_get_setPosition(self):
        """Test the setPosition() method."""
        cmd_pos = [0] * 6
        self.component.setPosition(cmd_pos, 0)
        print '\nsetPosition(%s, %s)' %(cmd_pos, 0)
        time.sleep(10)
        sdelay = 20
        delay = sdelay * 10 ** 7 # 20 seconds
        step =  2 * 10 ** 6 # 100 ms
        points = 100
        exe_time = TimeHelper.getTimeStamp().value + delay # Set the positions in delay seconds from now
        out_file = open('data/commanded_pos.data', 'w')

        for i in range(points): 
            cmd_pos[2] += 0.2 # increment the z axis
            future_time = exe_time + step * i
            self.component.setPosition(cmd_pos, future_time)
            length = len(str(future_time))
            time_seconds = str(future_time)[length-8:]
            out_file.write("Time: %s -- Position Z: %.4f\n" %(time_seconds, cmd_pos[2]))
            # time.sleep(0.1)

        actual_time = TimeHelper.getTimeStamp().value 
        while actual_time < exe_time - 4:
            actual_time = TimeHelper.getTimeStamp().value 
            time.sleep(1)

        out_file = open('data/linear_movement.data', 'w')
        while actual_time < exe_time + points * step:
            actPos_obj = self.component._get_actPos()
            actPos, cmp = actPos_obj.get_sync()
            actual_time = TimeHelper.getTimeStamp().value
            length = len(str(actual_time))
            time_seconds = str(actual_time)[length-8:]
            out_file.write("Time: %s -- Position Z: %.4f\n" %(time_seconds, actPos[2]))
            time.sleep(0.04)

if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestPositioning)
    unittest.TextTestRunner(verbosity=2).run(suite)
    print "\n" + "="*70

