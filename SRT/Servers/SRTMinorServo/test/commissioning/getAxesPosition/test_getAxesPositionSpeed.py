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

    def test_getAxesPositionSpeed(self):
        """Test the getAxesPosition() speed."""
        deltas = []
        for i in range(500):
            t = TimeHelper.getTimeStamp().value 
            positions = self.boss.getAxesPosition(t-1000000)
            delta = TimeHelper.getTimeStamp().value - t
            deltas.append(delta)

        average = 1.0 * sum(deltas)/len(deltas) / (10**7) 
        print
        print 'Initial delay: ', 1.0 * deltas[5] / 10**7
        print 'Final delay: ', 1.0 * deltas[-1] / 10**7
        print 'Max: ', 1.0 * max(deltas) / 10**7
        print 'Min: ', 1.0 * min(deltas) / 10**7
        print 'Average: ', average

if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestGetAxesPosition)
    unittest.TextTestRunner(verbosity=2).run(suite)
    print "\n" + "="*70

