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
        self.component_name = "MINORSERVO/PFP"
        self.component = self.pyclient.getComponent(self.component_name)

    def _tearDownClass():
        print "in tearDown"
        self.sockobj.close()
        self.pyclient.releaseComponent(self.component_name)

    def test_get_setPosition(self):
        """Test the setPosition() method."""
        cmd_pos = [0] * 3
        self.component.setPosition(cmd_pos, 0)
        print '\nsetPosition(%s, %s)' %(cmd_pos, 0)
        time.sleep(10)
        delay = 10 * 10 ** 7 # 10 seconds
        step =  2 * 10 ** 6 # 200 ms
        exe_time = TimeHelper.getTimeStamp().value + delay # Set the positions in delay seconds from now
        for i in range(100): # 20 seconds
            for idx, item in enumerate(cmd_pos):
                cmd_pos[idx] += 0.2
            print '\nsetPosition(%s, %s)' %(cmd_pos, exe_time + delay + step * i)
            self.component.setPosition(cmd_pos, exe_time + delay  + step * i)
            time.sleep(0.2)


if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestPositioning)
    unittest.TextTestRunner(verbosity=2).run(suite)
    print "\n" + "="*70

