#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.
"""Use this module to test the minor servo boss checkFocusScan() method"""

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


def isAlmostEqual(seq1, seq2, delta = 0.2):
    if len(seq1) != len(seq2):
        return False
    else:
        for a, b in zip(seq1, seq2):
            if abs(a-b) > delta:
                return False
    return True


class TestCheckFocusScan(unittest.TestCase):
    """Test the minor servo checkFocusScan() method."""

    def setUp(self):
        self.pyclient = PySimpleClient()
        self.cmd_num = 0
        self.boss = self.pyclient.getComponent("MINORSERVO/Boss")
        self.srp = self.pyclient.getComponent("MINORSERVO/SRP")
        self.max_speed = 4 # mm/s
        self.acceleration = 8 # mm/s^2
        self.acceleration_time = 2.0 * self.max_speed / self.acceleration # time in acceleration
        self.acceleration_distance = 1.0 * self.max_speed ** 2 / self.acceleration # space in acceleration
        self.shift_time = 2 # seconds
        self.guard_coeff = 1.2 
        self.delay = 80 * 10 ** 6 # 8 seconds
        self.axis_id = 2
        self.setup_code = 'CCB'

    def tearDownClass():
        print "in tearDown"
        self.sockobj.close()
        self.pyclient.releaseComponent(self.boss)
        self.pyclient.releaseComponent(self.srp)

    def test_positioning(self):
        """Test if checkFocusScan() computes in the right way the positioning time."""
        if not self.boss.isReady() or not self.boss.isTracking():
            self.boss.setup(self.setup_code)
            print "\nExecuting the %s setup. Wait a bit ..." %self.setup_code
            time.sleep(1) # Wait a bit, until the boss begins the configuration process
        
        counter = 0
        delay_ready = 2
        while not self.boss.isReady() or not self.srp.isReady(): # Wait until the minor servo boss is ready
            time.sleep(delay_ready) # Wait a bit, until the boss is active
            counter += delay_ready
            if counter > 240:
                self.assertTrue(counter > 240)
                return

        print "\nThe MinorServoBoss is ready."

        time.sleep(2)
        self.delay = 48 * 10 ** 6 # 4.8 seconds
        starting_time = TimeHelper.getTimeStamp().value + self.delay # Check for the scan in `delay` seconds from now
        range_ = 10.0 # mm 
        total_time = 10 * 10 ** 7 # 10 seconds
        actPos_obj = self.srp._get_actPos()
        actPos, cmp = actPos_obj.get_sync()
        actual_pos = actPos[self.axis_id]
        left_pos = self.boss.getCentralScanPosition() - range_/2 if self.boss.isScanActive() else actual_pos - range_/2 
        right_pos = self.boss.getCentralScanPosition() + range_/2 if self.boss.isScanActive() else actual_pos + range_/2 
        target_pos = left_pos if abs(actual_pos - left_pos) <= abs(actual_pos - right_pos) else right_pos
        distance = abs(target_pos - actual_pos)
        min_time = self.shift_time + (self.acceleration_time + (distance - self.acceleration_distance) / self.max_speed)*self.guard_coeff
        min_time += 1.0 # The time the method getMinScanStartingTime needs to execute
        print "Check if it can reach the starting position (%s mm of distance) for the starting time" %distance
        print "Expected min time to reach the starting scan position: %s" %min_time
        checkScan_min_time = (starting_time - TimeHelper.getTimeStamp().value)/(1.0 * 10**7)
        for i in range(1, 20, 1):
            if self.boss.checkFocusScan(starting_time, range_, total_time):
                print TimeHelper.getTimeStamp().value 
                print "Min time from checkFocusScan(): %s" %checkScan_min_time
                break
            else:
                print "\tIt cannot reach the position in %s" %checkScan_min_time
                now = TimeHelper.getTimeStamp().value 
                starting_time = now + self.delay + i * 10**6
                checkScan_min_time = (starting_time - now)/(1.0 * 10**7)
                time.sleep(0.1)

        self.assertTrue(min_time - 1.2 < checkScan_min_time < min_time + 1.2)
        
if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestCheckFocusScan)
    unittest.TextTestRunner(verbosity=2).run(suite)
    print "\n" + "="*70

