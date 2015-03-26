from __future__ import with_statement

import os
import math
import time
import datetime

import unittest2

import MinorServo
import Management
import Antenna

from Acspy.Clients.SimpleClient import PySimpleClient
from MinorServoErrors import MinorServoErrorsEx
from Acspy.Common.TimeHelper import getTimeStamp

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class TestGetAxesInfo(unittest2.TestCase):

    def setUp(self):
        self.telescope = os.getenv('TARGETSYS')
        self.client = PySimpleClient()
        self.boss = self.client.getComponent('MINORSERVO/Boss')
        self.setup_code = "CCB" if self.telescope == "SRT" else "CCC"

    def tearDown(self):
        self.client.releaseComponent('MINORSERVO/Boss')

    def test_not_ready(self):
        """Raise a MinorServoErrorsEx in case the system is not ready"""
        with self.assertRaises(MinorServoErrorsEx):
            axes, units = self.boss.getAxesInfo()

    def test_ready(self):
        """Get the axes information"""
        self.boss.setup(self.setup_code)
        counter = 0 # Seconds
        now = time_ref = datetime.datetime.now()
        while not self.boss.isReady() or (time_ref - now).seconds < 20:
            time.sleep(1)
            now = datetime.datetime.now()

        axes, units = self.boss.getAxesInfo()
        self.assertTrue(any(axes))
        self.assertTrue(any(units))
        self.assertEqual(len(units), len(axes))


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest2.main() # Real test using the antenna CDB
    else:
        from PyMinorServoTest import simunittest
        simunittest.run(TestGetAxesInfo)

