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


class TestSystemOffset(unittest2.TestCase):

    telescope = os.getenv('TARGETSYS')

    @classmethod
    def setUpClass(cls):
        cls.client = PySimpleClient()
        cls.boss = cls.client.getComponent('MINORSERVO/Boss')
        
    @classmethod
    def tearDownClass(cls):
        cls.client.releaseComponent('MINORSERVO/Boss')

    def setUp(self):
        self.setup_code = "CCB" if self.telescope == "SRT" else "CCC"
        self.axis_code = "SRP_TX" if self.telescope == "SRT" else "X"

    def tearDown(self):
        #self.boss.clearSystemOffset(self.axis_code)
        self.boss.setSystemOffset(self.axis_code, 0)  # TODO
        self.boss.park()
        time.sleep(0.2)
        self.wait_parked()

    def test_wrong_servo_name(self):
        """Raise a MinorServoErrorsEx in case of wrong servo name"""
        self.boss.setup(self.setup_code)
        counter = 0 # Seconds
        now = time_ref = datetime.datetime.now()
        while not self.boss.isReady() or (time_ref - now).seconds < 20:
            time.sleep(1)
            now = datetime.datetime.now()
        with self.assertRaises(MinorServoErrorsEx):
            self.boss.setSystemOffset(self.axis_code + "WRONG", 0)

    def test_get_offset(self):
        self.boss.setup(self.setup_code)
        counter = 0 # Seconds
        now = time_ref = datetime.datetime.now()
        while not self.boss.isReady() or (time_ref - now).seconds < 20:
            time.sleep(1)
            now = datetime.datetime.now()

        target_offset = 5.0
        self.boss.setSystemOffset(self.axis_code, target_offset)
        offset = self.boss.getSystemOffset()[0] # SRP_TX and X both have index 0
        self.assertAlmostEqual(offset, target_offset, delta=0.1)

    def wait_parked(self):
        while self.boss.isParking():
            time.sleep(0.1)


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest2.main() # Real test using the antenna CDB
    else:
        from PyMinorServoTest import simunittest
        simunittest.run(TestSystemOffset)
