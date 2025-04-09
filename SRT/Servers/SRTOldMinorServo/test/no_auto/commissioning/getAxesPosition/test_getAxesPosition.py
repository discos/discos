#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.
"""Use this module to test the minor servo boss getAxesPosition() method"""

import unittest
import threading
import os
import time
from random import randrange as rr
from socket import *       
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common import TimeHelper
from maciErrTypeImpl import CannotGetComponentExImpl

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

class TestGetAxesPosition(unittest.TestCase):
    """Test the minor servo properties."""

    def setUp(self):
        self.pyclient = PySimpleClient()
        self.cmd_num = 0
        self.srp = self.pyclient.getComponent("MINORSERVO/SRP")
        self.boss = self.pyclient.getComponent("MINORSERVO/Boss")

    def tearDownClass():
        print "in tearDown"
        self.sockobj.close()
        self.pyclient.releaseComponent(self.srp)
        self.pyclient.releaseComponent(self.boss)

    def test_getAxesPosition(self):
        """Test the getAxesPosition() method."""
        print
        print "Performing a setup..."
        self.boss.setup('CCB')
        while not self.boss.isReady():
            time.sleep(2)

        time.sleep(2)
        print "Setup done!"
        cmdPos = [0] * 6
        self.srp.setPosition(cmdPos, 0)
        print '\nsetPosition(%s, %s)' %(cmdPos, 0)

        actPos_obj = self.srp._get_actPos()
        actPos, cmp = actPos_obj.get_sync()
        print "Going to the target position..."
        while not isAlmostEqual(actPos, cmdPos):
            time.sleep(2)
            print "actPos: ", actPos
            actPos, cmp = actPos_obj.get_sync()
            
        print "Done!"

        delay = 20 * 10 ** 7 # 20 seconds
        step =  2 * 10 ** 6 # 200 ms
        points = 40
        exe_time = TimeHelper.getTimeStamp().value + delay # Set the positions in delay seconds from now
        print "Setting the positions..."
        self.srp.setPosition(cmdPos, exe_time)
        for i in range(points): 
            cmdPos[2] += 0.2 # increment the z axis
            future_time = exe_time + step * (i + 1)
            self.srp.setPosition(cmdPos, future_time)
        print "Done!"

        actual_time = TimeHelper.getTimeStamp().value 
        print "Waiting the exe_time..."
        positions = {}
        while actual_time < exe_time - 2*10**7:
            actual_time = TimeHelper.getTimeStamp().value 
            actPos, cmp = actPos_obj.get_sync()
            print "actPos: ", actPos
            time.sleep(1)

        print "The servo is moving..."
        while actual_time < exe_time + points * step:
            actPos, cmp = actPos_obj.get_sync()
            print "actPos", actPos
            actual_time = TimeHelper.getTimeStamp().value 
            positions[actual_time] = actPos[:]
            time.sleep(0.200)
        print "Done!"

        print "Writing the file..."
        out_file = open('../data/axesPositionHistory.data', 'w')
        hpositions = {}
        for t in sorted(positions.keys()):
            print "time: ", t
            rp = ' '.join(['%.4f' %item for item in positions[t]])
            hp = self.boss.getAxesPosition(t)[0:6]
            out_file.write("\n" + "-"*68)
            out_file.write("\nTime: %s" %t)
            out_file.write("\nExpected: %s" %rp)
            out_file.write("\nHistory:  %s" %' '.join(['%.4f' %item for item in hp]))
            hpositions[t] = hp
            print "-"*40
            print "Timestamp: ", t
            print "History : ", hp
            print "Expected: ", rp
            print
        pos = [positions[t] for t in sorted(positions.keys())]
        hpos = [hpositions[t] for t in sorted(hpositions.keys())]
        max_diff = max([abs(p[2] - h[2]) for p, h in zip(pos[4:], hpos[4:])])
        out_file.write("\n\nMax diff: %.4f" %max_diff)
        print "Max diff [4:]: ", max_diff
        print "Done!"





if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestGetAxesPosition)
    unittest.TextTestRunner(verbosity=2).run(suite)
    print "\n" + "="*70

