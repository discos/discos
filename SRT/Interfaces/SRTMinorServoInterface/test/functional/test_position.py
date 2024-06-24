from __future__ import with_statement
import random 
import math
import time
import os
from datetime import datetime

import unittest
import Management
import MinorServo
import Antenna

from MinorServoErrors import MinorServoErrorsEx
from Acspy.Common.TimeHelper import getTimeStamp
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Util import ACSCorba

from acswrapper.system import acs
from acswrapper.containers import (
    Container, ContainerError, start_containers_and_wait,
    stop_containers_and_wait
)

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class PositionTest(unittest.TestCase):

    telescope = os.getenv('STATION')

    @classmethod
    def setUpClass(cls):
        if not acs.is_running():
            acs.start()
        cls.containers = [
            Container('MinorServoContainer', 'cpp'),
            Container('MinorServoBossContainer', 'cpp'),
        ]
        try:
            start_containers_and_wait(cls.containers)
        except ContainerError, ex:
            cls.fail(ex.message)
        cls.client = PySimpleClient()
        cls.boss = cls.client.getComponent('MINORSERVO/Boss')
        
    @classmethod
    def tearDownClass(cls):
        cls.client.releaseComponent('MINORSERVO/Boss')
        stop_containers_and_wait(cls.containers)

    def setUp(self):
        self.axis_code='SRP_TZ' if self.telescope == 'SRT' else 'Z'
        setupCode = 'KKG' if self.telescope == 'SRT' else 'CCC'
        # Wait (maximum one minute) in case the boss is parking
        if self.boss.isParking():
            t0 = datetime.now()
            while self.boss.isParking() and (datetime.now() - t0).seconds < 60:
                time.sleep(2)
            if self.boss.isParking():
                self.fail('The system can not exit form a parking state')

        if self.boss.getActualSetup() != setupCode:
            self.boss.setup(setupCode)
            # Wait (maximum 5 minutes) in case the boss is starting
            t0 = datetime.now()
            while not self.boss.isReady() and (datetime.now() - t0).seconds < 60*5:
                time.sleep(2)
            if not self.boss.isReady():
                self.fail('The system is not ready for executing the tests')
        self.boss.setElevationTracking('OFF')
        self.boss.setASConfiguration('OFF')
        axes, units = self.boss.getAxesInfo()
        self.idx = axes.index(self.axis_code)

    def tearDown(self):
        # self.boss.clearUserOffset(self.axis_code)
        self.boss.setUserOffset(self.axis_code, 0)
        self.wait_tracking()

    def test_get_current_position(self):
        timestamp = getTimeStamp().value
        position = self.get_position()
        position_now = self.get_position(timestamp)
        self.assertAlmostEqual(position, position_now, delta=0.1)

    def test_get_offset_position(self):
        position = self.get_position()
        self.boss.setUserOffset(self.axis_code, 10)
        self.wait_tracking()
        position_now = self.get_position()
        self.assertAlmostEqual(position_now, position + 10, delta=0.1)

    def test_get_past_position(self):
        timestamp = getTimeStamp().value
        position = self.get_position()
        self.boss.setUserOffset(self.axis_code, 10)
        self.wait_tracking()
        position_past = self.get_position(timestamp)
        self.assertAlmostEqual(position, position_past, delta=0.1)

    def test_get_past_position_with_sleep(self):
        timestamp = getTimeStamp().value
        position = self.get_position()
        self.boss.setUserOffset(self.axis_code, 10)
        self.wait_tracking()
        time.sleep(10)
        position_past = self.get_position(timestamp)
        self.assertAlmostEqual(position, position_past, delta=0.1)

    def wait_tracking(self):
        time.sleep(1) # Give the time to command the new position
        # TODO: we need a better solution than this sleep to be sure the tests
        # procuce always the same results
        while not self.boss.isTracking():
            time.sleep(0.1)

    def get_position(self, timestamp=0):
        return self.boss.getAxesPosition(timestamp)[self.idx]
        

if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest.main(verbosity=2, failfast=True) # Real test using the antenna CDB
    else:
        from testing import simulator
        simulator.run(PositionTest, 'srt-mscu-sim')
