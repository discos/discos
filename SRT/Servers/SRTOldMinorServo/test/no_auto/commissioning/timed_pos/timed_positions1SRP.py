#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.
"""Use this module to test the minor servo setPosition() method, with `exe_time != 0`.
   The positions are commanded one each `sdelay` seconds."""

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
        sdelay = 8L # 8 seconds
        delay = sdelay * 10 ** 7 
        exe_time = TimeHelper.getTimeStamp().value + delay # Set the position in delay seconds from now
        cmd_pos = (0,) * 6
        for i in range(8):
            print '\nsetPosition(%s, %s)' %(cmd_pos, exe_time + delay * i)
            self.component.setPosition(cmd_pos, exe_time + delay * i)
            time.sleep(sdelay)

            while True:
                if self.component.isTracking():
                    print 'Position reached!'
                    break
                time.sleep(1)
                    
            time.sleep(2) # Wait till the position is stable
            pos_obj = self.component._get_actPos()
            act_pos, cmp = pos_obj.get_sync()

            for cpi, api in zip(cmd_pos, act_pos):
                self.assertAlmostEqual(cpi, api, places=1)

            cmd_pos = (rr(-5, 6), rr(-5, 6), rr(-5, 6), rr(-5, 6) / 100.0, rr(-5, 6) / 100.0, rr(-5, 6)/ 100.0)


if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestPositioning)
    unittest.TextTestRunner(verbosity=2).run(suite)
    print "\n" + "="*70

