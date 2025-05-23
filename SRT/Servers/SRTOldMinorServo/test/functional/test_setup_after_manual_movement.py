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


class CannotSetupTest(unittest.TestCase):

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
        cls.pfp = cls.client.getComponent('MINORSERVO/PFP')
        
    @classmethod
    def tearDownClass(cls):
        cls.client.releaseComponent('MINORSERVO/Boss')
        cls.client.releaseComponent('MINORSERVO/PFP')
        cls.client.disconnect()
        stop_containers_and_wait(cls.containers)

    def test_setup_after_manual_movement(self):
        """Verify the setup completes after a manual movement"""
        self.boss.setup('XKP')
        self.wait_until_ready()
        # XKP pos: PFP: RY(mm)=(-25.75); TX(mm)=(-841.15); TZ(mm)=(-45.9);
        target_position = [-25, -900, -47]
        self.pfp.setPosition(target_position, 0)
        self.wait_until(self.pfp.isTracking)
        position = self.pfp.getPositionFromHistory(0)
        # self.assertSequenceEqual(target_position, position)
        self.boss.setup('LLP')
        self.wait_until_ready()
        self.assertEqual(self.boss.getActualSetup(), 'LLP')

    def wait_until(self, is_true, timeout=240):
        time.sleep(0.5)
        t0 = datetime.now()
        while not is_true():
            if (datetime.now() - t0).seconds > timeout:
                assert False, 'Cannot wait_until %s' %is_true
            else:
                time.sleep(1)
        time.sleep(0.5)

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
        simulator.run(CannotSetupTest, 'srt-mscu-sim')
