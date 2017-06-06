from __future__ import with_statement

import os
import math
import time
import unittest

import MinorServo
import Management
import Antenna

from Acspy.Clients.SimpleClient import PySimpleClient
from MinorServoErrors import MinorServoErrorsEx
from Acspy.Common.TimeHelper import getTimeStamp

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class TestSetServoElevationTrackingCmd(unittest.TestCase):
    """Test the setServoElevationTracking command"""

    telescope = os.getenv('STATION')

    def setUp(self):
        self.client = PySimpleClient()
        self.boss = self.client.getComponent('MINORSERVO/Boss')

    def tearDown(self):
        self.client.releaseComponent('MINORSERVO/Boss')

    def test_wrong_flag(self):
        success, answer = self.boss.command('setServoElevationTracking=FOO')
        self.assertFalse(success)

    def test_right_flag(self):
        #FIXME: we need servoSetup before this
        success, answer = self.boss.command('setServoElevationTracking=on')
        self.assertTrue(success)


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest.main() # Real test using the antenna CDB
    else:
        from PyMinorServoTest import simunittest
        simunittest.run(TestSetServoElevationTrackingCmd)
