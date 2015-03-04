from __future__ import with_statement

import os
import math
import time
import datetime
import unittest2

import MinorServo
import Management
import Antenna

from PyMinorServoTest import simunittest
from Acspy.Clients.SimpleClient import PySimpleClient
from MinorServoErrors import MinorServoErrorsEx
from Acspy.Common.TimeHelper import getTimeStamp

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class TestSystemOffset(unittest2.TestCase):

    def setUp(self):
        self.client = PySimpleClient()
        self.boss = self.client.getComponent('MINORSERVO/Boss')

    def tearDown(self):
        self.client.releaseComponent('MINORSERVO/Boss')

    def test_wrong_servo_name(self):
        """Raise a MinorServoErrorsEx in case of wrong servo name"""
        with self.assertRaises(MinorServoErrorsEx):
            self.boss.setSystemOffset('SRP_TX', 0)

    def test_get_offset(self):
        self.boss.setup('CCB')
        counter = 0 # Seconds
        now = time_ref = datetime.datetime.now()
        while not self.boss.isReady() or (time_ref - now).seconds < 20:
            time.sleep(1)
            now = datetime.datetime.now()

        target_offset = 5.0
        self.boss.setSystemOffset('SRP_TX', target_offset)
        offset = self.boss.getSystemOffset()[0] # SRP_TX has index 0
        self.assertAlmostEqual(offset, target_offset, delta=0.1)


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest2.main() # Real test using the antenna CDB
    else:
        simunittest.run(TestSystemOffset)
