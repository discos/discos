from __future__ import with_statement

import os
import math

import unittest2

import MinorServo
import Management
import Antenna

from PyMinorServoTest import simunittest
from Acspy.Clients.SimpleClient import PySimpleClient
from MinorServoErrors import MinorServoErrorsEx
from Acspy.Common.TimeHelper import getTimeStamp

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class TestCloseScan(unittest2.TestCase):

    def setUp(self):
        client = PySimpleClient()
        self.boss = client.getComponent('MINORSERVO/Boss')

    def test_scan_not_active(self):
        """Raise a MinorServoErrorsEx in case no scan is active"""
        with self.assertRaises(MinorServoErrorsEx):
            time_to_stop = self.boss.closeScan() 


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest2.main() # Real test using the antenna CDB
    else:
        simunittest.run(TestCloseScan)

