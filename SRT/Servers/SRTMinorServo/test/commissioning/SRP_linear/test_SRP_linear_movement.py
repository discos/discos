#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.
"""Use this module to test if the SRP movement is `linear`."""

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


class TestLinearMovement(unittest.TestCase):
    """Test the minor servo properties."""

    def setUp(self):
        self.pyclient = PySimpleClient()
        self.cmd_num = 0
        self.component_name = "MINORSERVO/SRP"
        self.component = self.pyclient.getComponent(self.component_name)

    def _tearDownClass():
        print "in tearDown"
        self.sockobj.close()
        self.pyclient.releaseComponent(self.component_name)

    def test_SRPLinearMovement(self):
        """Test if the SRP movement is linear."""
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
        step =  2 * 10 ** 7 # 1 second
        points = 10
        increment = 2.0 # mm
        exe_time = TimeHelper.getTimeStamp().value + delay # Set the positions in delay seconds from now
        print "Setting the positions..."
        commanded_positions = {}
        self.component.setPosition(cmdPos, exe_time)
        commanded_positions[exe_time] = cmdPos[:] 
        starting_pos = cmdPos[:]
        for i in range(points): 
            cmdPos[2] += increment # increment the z axis
            future_time = exe_time + step * (i + 1)
            self.component.setPosition(cmdPos, future_time)
            commanded_positions[future_time] = cmdPos[:]
        print "Done!"

        expected_positions = {}
        sampling_time_step = 2 * 10 ** 6 # 2ms
        scale_factor = step / sampling_time_step # Integer result
        sampling_points = points * scale_factor
        position = starting_pos
        expected_positions[exe_time] = position[:]
        for i in range(sampling_points):
            position[2] += increment / scale_factor # float result
            next_time = exe_time + sampling_time_step * (i + 1)
            expected_positions[next_time] = position[:]
        for j in range(4):
            next_time = exe_time + sampling_time_step * (i + 2 + j)
            expected_positions[next_time] = position[:]
            

        actual_time = TimeHelper.getTimeStamp().value 
        print "Waiting the exe_time..."
        while actual_time < exe_time - 2*10**7:
            actual_time = TimeHelper.getTimeStamp().value 
            actPos, cmp = actPos_obj.get_sync()
            print "actPos: ", actPos
            time.sleep(1)

        print "The servo is moving..."
        while actual_time < exe_time + points * step:
            actPos, cmp = actPos_obj.get_sync()
            print "actPos", actPos
            time.sleep(1)
            actual_time = TimeHelper.getTimeStamp().value 
        print "Done!"

        print "Writing the file..."
        out_file = open('../data/linear_movement.data', 'w')
        keys = sorted(commanded_positions.keys())
        out_file.write('# Running time: %s\n\n' %str(datetime.datetime.now()))
        out_file.write('# Positions (Z) from %.4f (mm) to %.4f (mm)\n' %(commanded_positions[keys[0]][2], commanded_positions[keys[-1]][2]))
        out_file.write('# Time step between adiacent positions: %d (ms)\n' %(step / (10 ** 4)))
        out_file.write('# starting_time: %d\n' %exe_time)
        for t in sorted(commanded_positions.keys()):
            t_diff_ms = (t - exe_time) / (1.0 * 10**4)
            out_file.write('# pos(Z) @ starting_time + %0*.3f (ms) -> %0*.3f (mm)\n' %(9, t_diff_ms, 6, commanded_positions[t][2]))

        hpositions = {}
        for t in sorted(expected_positions.keys()):
            print "time: ", t
            ep = ' '.join(['%.4f' %item for item in expected_positions[t]])
            hp = self.component.getPositionFromHistory(t)
            out_file.write("\n#" + "-"*68)
            out_file.write("\nTime: exe_time + %0*d ms" %(5, (t - exe_time) / (10**4)))
            out_file.write("\nExpected: %s" %ep)
            out_file.write("\nHistory:  %s" %' '.join(['%.4f' %item for item in hp]))
            hpositions[t] = hp
            print "-"*40
            print "Timestamp: ", t
            print "History : ", hp
            print "Expected: ", ep
            print
        pos = [expected_positions[t] for t in sorted(expected_positions.keys())]
        hpos = [hpositions[t] for t in sorted(hpositions.keys())]
        max_diff = max([abs(p[2] - h[2]) for p, h in zip(pos, hpos)])
        out_file.write("\n\n# Max diff: %.4f" %max_diff)
        print "Max diff: ", max_diff
        print "Done!"


if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestLinearMovement)
    unittest.TextTestRunner(verbosity=2).run(suite)
    print "\n" + "="*70

