from __future__ import with_statement
import random 
import math
import time
import os
from datetime import datetime
from subprocess import Popen, PIPE

import unittest
import Management
import MinorServo
import Antenna

from MinorServoErrors import MinorServoErrorsEx
from Acspy.Common.TimeHelper import getTimeStamp
from Acspy.Clients.SimpleClient import PySimpleClient

from acswrapper.system import acs
from acswrapper.containers import (
    Container, ContainerError, start_containers_and_wait,
    stop_containers_and_wait
)


__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"

class SetupTest(unittest.TestCase):

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

    @classmethod
    def tearDownClass(cls):
        stop_containers_and_wait(cls.containers)

    def setUp(self):
        self.client = PySimpleClient()
        self.boss = self.client.getComponent('MINORSERVO/Boss')

    def tearDown(self):
        self.boss.park()
        self.wait_until_not_ready()
        self.client.releaseComponent('MINORSERVO/Boss')

    def test_elevation_tracking_ON(self):
        """The setup turns the elevation tracking on"""
        self.boss.setup('KKG')
        self.wait_until_ready()
        self.assertTrue(self.boss.isElevationTrackingEn())

    def test_as_configuration_ON(self):
        """The setup turns the AS configuration on"""
        self.boss.setup('KKG')
        self.wait_until_ready()
        self.assertEqual(self.boss.getActualSetup(), 'KKG_ASACTIVE')

    def test_do_not_turn_PF_elevation_tracking_ON(self):
        """In the Primary Focus, do not turn the elevation tracking on"""
        self.boss.setup('LLP')
        self.wait_until_ready()
        self.assertFalse(self.boss.isElevationTrackingEn())

    def test_do_not_turn_PF_as_configuration_ON(self):
        """In the Primary Focus, do not turn the AS configuration on"""
        self.boss.setup('LLP')
        self.wait_until_ready()
        self.assertEqual(self.boss.getActualSetup(), 'LLP')

    def test_actual_setup_unknown(self):
        self.boss.setup('KKG')
        self.wait_until_ready()
        self.boss.setup('KKG')
        self.assertEqual(self.boss.getActualSetup(), 'unknown')
        self.wait_until_ready() # Becase the tearDown() executes a park

    def wait_until_ready(self, timeout=240):
        time.sleep(0.5)
        t0 = datetime.now()
        while not self.boss.isReady():
            if (datetime.now() - t0).seconds > timeout:
                assert False, 'The minor servo boss is not ready'
            else:
                time.sleep(1)
        time.sleep(0.5)

    def wait_until_not_ready(self, timeout=240):
        time.sleep(0.5)
        t0 = datetime.now()
        while self.boss.isReady():
            if (datetime.now() - t0).seconds > timeout:
                assert False, 'The minor servo boss is not parked'
            else:
                time.sleep(1)
        time.sleep(0.5)


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest.main(verbosity=2, failfast=True) # Real test using the antenna CDB
    else:
        from testing import simulator
        simulator.run(SetupTest, 'srt-mscu-sim')
