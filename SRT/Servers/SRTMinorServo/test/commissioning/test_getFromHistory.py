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

def isAlmostEqual(seq1, seq2, delta = 0.2):
    if len(seq1) != len(seq2):
        return False
    else:
        for a, b in zip(seq1, seq2):
            if abs(a-b) > delta:
                return False
    return True


class TestPositionFromHistory(unittest.TestCase):
    """Test the minor servo properties."""

    def setUp(self):
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

    def test_getPositionFromHistory(self):
        """Test the getPositionFromHistory() method."""
        print
        print "Performing a setup..."
        self.component.setup(0)
        while not self.component.isReady():
            time.sleep(2)

        time.sleep(2)
        print "Setup done!"
        cmdPos = [0] * 6
        self.component.setPosition(cmdPos, 0)
        print '\nsetPosition(%s, %s)' %(cmdPos, 0)

        actPos_obj = self.component._get_actPos()
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
        self.component.setPosition(cmdPos, exe_time)
        for i in range(points): 
            cmdPos[2] += 0.2 # increment the z axis
            future_time = exe_time + step * i
            self.component.setPosition(cmdPos, future_time)
        print "Done!"

        actual_time = TimeHelper.getTimeStamp().value 
        print "Waiting the exe_time..."
        while actual_time < exe_time - 2*10**7:
            actual_time = TimeHelper.getTimeStamp().value 
            actPos, cmp = actPos_obj.get_sync()
            print "actPos: ", actPos
            time.sleep(1)

        print "The servo is moving..."
        positions = {}
        while actual_time < exe_time + points * step:
            actPos, cmp = actPos_obj.get_sync()
            actual_time = TimeHelper.getTimeStamp().value
            positions[actual_time] = actPos
            print "actPos", actPos
            time.sleep(0.200)
        print "Done!"

        print "Writing the file..."
        out_file = open('data/history.data', 'w')
        hpositions = {}
        for t in sorted(positions.keys()):
            print "time: ", t
            rp = ' '.join(['%.4f' %item for item in positions[t]])
            hp = self.component.getPositionFromHistory(t)
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
    suite = unittest.TestLoader().loadTestsFromTestCase(TestPositionFromHistory)
    unittest.TextTestRunner(verbosity=2).run(suite)
    print "\n" + "="*70

