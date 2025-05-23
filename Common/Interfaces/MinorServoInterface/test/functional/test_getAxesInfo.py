from __future__ import with_statement

import os
import math
import time
import datetime

import unittest

import MinorServo
import Management
import Antenna

from Acspy.Clients.SimpleClient import PySimpleClient
from MinorServoErrors import MinorServoErrorsEx
from Acspy.Common.TimeHelper import getTimeStamp


from acswrapper.system import acs
from acswrapper.containers import (
    Container, ContainerError, start_containers_and_wait,
    stop_containers_and_wait
)

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class TestGetAxesInfo(unittest.TestCase):

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
        cls.client.disconnect()
        stop_containers_and_wait(cls.containers)

    def setUp(self):
        self.setup_code = "CCB" if self.telescope == "SRT" else "CCC"

    def tearDown(self):
        self.boss.park()
        time.sleep(0.2)
        self.wait_parked()

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

    def wait_parked(self):
        while self.boss.isParking():
            time.sleep(0.1)


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest.main() # Real test using the antenna CDB
    else:
        from testing import simulator
        simulator.run(TestGetAxesInfo, 'srt-mscu-sim')

