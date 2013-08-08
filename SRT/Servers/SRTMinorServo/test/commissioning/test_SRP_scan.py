#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.
"""Use this module to test the minor servo boss startScan() method"""

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

class TestScan(unittest.TestCase):
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
        self.srp = self.pyclient.getComponent("MINORSERVO/SRP")
        self.boss = self.pyclient.getComponent("MINORSERVO/Boss")

    def tearDownClass():
        print "in tearDown"
        self.sockobj.close()
        self.pyclient.releaseComponent(self.srp)
        self.pyclient.releaseComponent(self.boss)

    def test_startScan(self):
        """Test the startScan() method."""
        setup_code = 'CCB'
        self.boss.setup(setup_code)
        time.sleep(1) # Wait a bit, until the boss begins the configuration process
        
        counter = 0
        print "\nExecuting the minor servo %s setup. Wait a bit ..." %setup_code
        delay_ready = 2
        while not self.boss.isReady(): # Wait until the minor servo boss is ready
            time.sleep(delay_ready) # Wait a bit, until the boss is active
            counter += delay_ready
            if counter > 240:
                self.assertTrue(counter > 240)
                return

        print "\nThe MinorServoBoss is ready."

        time.sleep(10)
        delay = 10 * 10 ** 7 # 10 seconds
        starting_time = TimeHelper.getTimeStamp().value + delay # Start the scan in `delay` seconds from now
        range_ = 20 # mm 
        total_time = 10 * 10 ** 7 # 10 seconds
        self.boss.startScan(starting_time, range_, total_time, 2, "SRP")

        actual_time = TimeHelper.getTimeStamp().value 
        while actual_time < starting_time - 2:
            actual_time = TimeHelper.getTimeStamp().value 
            time.sleep(0.5)

        out_file = open('data/scan.data', 'w')
        while actual_time < starting_time + total_time -3:
            actPos_obj = self.srp._get_actPos()
            actPos, cmp = actPos_obj.get_sync()
            actual_time = TimeHelper.getTimeStamp().value
            length = len(str(actual_time))
            time_seconds = str(actual_time)[length-8:]
            out_file.write("Time: %s -- Position: %s\n" %(time_seconds, ["%.4f" %pos for pos in actPos]))
            time.sleep(0.04)

if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestScan)
    unittest.TextTestRunner(verbosity=2).run(suite)
    print "\n" + "="*70

