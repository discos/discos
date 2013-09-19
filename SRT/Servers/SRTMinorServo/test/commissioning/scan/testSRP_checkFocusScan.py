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
        starting_time = TimeHelper.getTimeStamp().value + self.delay # Check for the scan in `delay` seconds from now
        range_ = 40.0 # mm 
        total_time = 40 * 10 ** 7 # 10 seconds
        actPos_obj = self.srp._get_actPos()
        actPos, cmp = actPos_obj.get_sync()
        actual_pos = actPos[self.axis_id]
        left_pos = self.boss.getCentralScanPosition() - range_/2 if self.boss.isScanActive() else actual_pos - range_/2 
        right_pos = self.boss.getCentralScanPosition() + range_/2 if self.boss.isScanActive() else actual_pos + range_/2 
        target_pos = left_pos if abs(actual_pos - left_pos) <= abs(actual_pos - right_pos) else right_pos
        distance = abs(target_pos - actual_pos)
        min_time = self.shift_time + (self.acceleration_time + (distance - self.acceleration_distance) / self.max_speed)*self.guard_coeff
        print "Check if it can reach the starting position (%s mm of distance) for the starting time" %distance
        print "Excpected min time to reach the starting scan position: %s" %min_time
        checkScan_min_time = (starting_time - TimeHelper.getTimeStamp().value)/(1.0 * 10**7)
        for i in range(1, 10, 2):
            if self.boss.checkFocusScan(starting_time, range_, total_time):
                print "Min time from checkFocusScan(): %s" %checkScan_min_time
                break
            else:
                print "\tIt cannot reach the position in %s" %checkScan_min_time
                now = TimeHelper.getTimeStamp().value 
                starting_time = now + self.delay + i * 10**6
                checkScan_min_time = (starting_time - now)/(1.0 * 10**7)

        print "checkScan_min_time: %s" %checkScan_min_time
        self.assertTrue(min_time - 1 < checkScan_min_time < min_time + 1)


    def test_duration(self):
        """Test if checkFocusScan() computes in the right way the minimum scan duration."""
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

        range_ = 40.0 # mm 
        total_time = 11 * 10 ** 7 # 11 seconds
        starting_total_time = total_time
        min_time = (self.acceleration_time + (range_ - self.acceleration_distance) / self.max_speed)*self.guard_coeff
        print "Compute the minimum time required in order to perform a scan of %.1f mm" %(range_/2)
        print "Excpected min time to perform the scan: %s" %min_time
        checkScan_min_time = total_time/(1.0 * 10**7)
        for i in range(1, 20, 2):
            starting_time = TimeHelper.getTimeStamp().value + self.delay * 3
            if self.boss.checkFocusScan(starting_time, range_, total_time):
                checkScan_min_time = total_time/(1.0 * 10**7)
                print "Min time from checkFocusScan(): %s" %checkScan_min_time
                break
            else:
                print "\tIt cannot perform the scan in %s seconds" %(total_time/(1.0* 10**7))
                total_time = starting_total_time + i * 10**6

        self.assertTrue(min_time - 1 < checkScan_min_time < min_time + 1)


    def test_notAllowed(self):
        """Test if checkFocusScan() behavior when the positions are not allowed."""
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

        range_ = 40.0 # mm 
        total_time = 40 * 10 ** 7 # 40 seconds
        starting_total_time = total_time
        max_value = self.srp.getMaxPositions()[self.axis_id];
        min_value = self.srp.getMinPositions()[self.axis_id];

        targetPos = [0, 0, -85, 0, 0, 0]
        actPos_obj = self.srp._get_actPos()
        actPos, cmp = actPos_obj.get_sync()
        print "Going to the target position..."
        self.srp.setPosition(targetPos, 0)
        while not isAlmostEqual(actPos, targetPos):
            time.sleep(2)
            actPos, cmp = actPos_obj.get_sync()
            
        central_pos = actPos[self.axis_id]
        left_pos = self.boss.getCentralScanPosition() - range_/2 if self.boss.isScanActive() else central_pos - range_/2 
        right_pos = self.boss.getCentralScanPosition() + range_/2 if self.boss.isScanActive() else central_pos + range_/2 

        print "Excpected min position: %s" %min_value
        for i in range(0, 20):
            starting_time = TimeHelper.getTimeStamp().value + self.delay * 3
            new_total_time = total_time + i * 10**7
            new_range = range_ + i
            left_position = central_pos - new_range/2
            if self.boss.checkFocusScan(starting_time, new_range, new_total_time):
                print "Position %s allowed" %(left_position)
            else:
                print "Position %s not allowed" %(left_position)
                break

        self.assertTrue(min_value - 1 < left_position < min_value + 1)

        
if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestCheckFocusScan)
    unittest.TextTestRunner(verbosity=2).run(suite)
    print "\n" + "="*70

