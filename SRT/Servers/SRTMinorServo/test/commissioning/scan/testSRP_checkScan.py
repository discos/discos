#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.
"""Use this module to test the minor servo boss checkScan() method"""

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

class NackEx(Exception):
    pass

class TestCheckScan(unittest.TestCase):
    """Test the minor servo startFocusScan() method."""

    def setUp(self):
        self.pyclient = PySimpleClient()
        self.cmd_num = 0
        self.boss = self.pyclient.getComponent("MINORSERVO/Boss")
        self.srp = self.pyclient.getComponent("MINORSERVO/SRP")

    def tearDownClass():
        print "in tearDown"
        self.sockobj.close()
        self.pyclient.releaseComponent(self.boss)
        self.pyclient.releaseComponent(self.srp)

    def test_startScan(self):
        """Test the checkScan() method."""
        setup_code = 'CCB'
        if not self.boss.isReady():
            self.boss.setup(setup_code)
            print "\nExecuting the %s setup. Wait a bit ..." %setup_code
            time.sleep(1) # Wait a bit, until the boss begins the configuration process
        
        counter = 0
        delay_ready = 2
        while not self.boss.isReady(): # Wait until the minor servo boss is ready
            time.sleep(delay_ready) # Wait a bit, until the boss is active
            counter += delay_ready
            if counter > 240:
                self.assertTrue(counter > 240)
                return

        print "\nThe MinorServoBoss is ready."

        max_speed = 4 # mm/s
        acceleration = 8 # mm/s^2
        acceleration_time = 2 * max_speed / acceleration # time in acceleration
        acceleration_distance = max_speed ** 2 / acceleration # space in acceleration
        shift_time = 2 # seconds
        guard_coeff = 1.2 

        time.sleep(2)
        delay = 60 * 10 ** 6 # 60 seconds
        starting_time = TimeHelper.getTimeStamp().value + delay # Check for the scan in `delay` seconds from now
        range_ = 40.0 # mm 
        total_time = 20 * 10 ** 7 # 20 seconds
        mm_per_sec = range_ / total_time
        actPos_obj = self.srp._get_actPos()
        actPos, cmp = actPos_obj.get_sync()
        axis_id = 2
        actual_pos = actPos[axis_id]
        target_pos = actual_pos - range_/2
        distance = abs(target_pos - actual_pos)
        min_time = shift_time + (acceleration_time + (range_/2 - acceleration_distance) / 4)*guard_coeff
        print "Check if it can do %.1f mm in %s seconds" %(distance, total_time / 10**7)
        print "Excpected min time to reach the position: %s" %min_time
        print "Starting the check..."
        for i in range(1, 10):
            if self.boss.checkScan(starting_time, range_, total_time, axis_id, "SRP"):
                print "\tmin time from checkScan(): %s" %((starting_time - TimeHelper.getTimeStamp().value)/(1.0 * 10**7))
                break
            else:
                print "\tIt cannot reach the position in %s" %((starting_time - TimeHelper.getTimeStamp().value)/(1.0 * 10**7))
                starting_time = TimeHelper.getTimeStamp().value + delay + i * 10**7
        # TODO: nice print of the result

        #-------------------------------
        # TODO: test the other conditions


if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestCheckScan)
    unittest.TextTestRunner(verbosity=2).run(suite)
    print "\n" + "="*70

