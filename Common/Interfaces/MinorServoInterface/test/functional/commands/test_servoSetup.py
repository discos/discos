from __future__ import with_statement

import os
import math
import time
import unittest2

import MinorServo
import Management
import Antenna

from Acspy.Clients.SimpleClient import PySimpleClient
from MinorServoErrors import MinorServoErrorsEx
from Acspy.Common.TimeHelper import getTimeStamp

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class TestServoSetupCmd(unittest2.TestCase):
    """Test the servoSetup command"""

    def setUp(self):
        self.telescope = os.getenv('TARGETSYS')
        self.client = PySimpleClient()
        self.boss = self.client.getComponent('MINORSERVO/Boss')
        self.setup_code = "CCB" if self.telescope == "SRT" else "CCC"

    def tearDown(self):
        self.client.releaseComponent('MINORSERVO/Boss')

    def test_wrong_code(self):
        success, answer = self.boss.command('servoSetup=FOO')
        self.assertFalse(success)

    def test_right_code(self):
        success, answer = self.boss.command('servoSetup=' + self.setup_code)
        self.assertTrue(success)


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest2.main() # Real test using the antenna CDB
    else:
        from PyMinorServoTest import simunittest
        simunittest.run(TestServoSetupCmd)
