from __future__ import with_statement

import os
import math
import time
import unittest2

import MinorServo
import Management
import Antenna

from PyMinorServoTest import simunittest
from Acspy.Clients.SimpleClient import PySimpleClient
from MinorServoErrors import MinorServoErrorsEx
from Acspy.Common.TimeHelper import getTimeStamp

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class TestSetServoASConfigurationCmd(unittest2.TestCase):
    """Test the setServoASConfiguration command"""

    telescope = os.getenv('TARGETSYS')

    def setUp(self):
        self.client = PySimpleClient()
        self.boss = self.client.getComponent('MINORSERVO/Boss')

    def tearDown(self):
        self.client.releaseComponent('MINORSERVO/Boss')

    def test_wrong_axis_code(self):
        success, answer = self.boss.command('setServoASConfiguration=FOO')
        self.assertFalse(success)

    def test_right_axis_code(self):
        success, answer = self.boss.command('setServoASConfiguration=on')
        self.assertTrue(success)

if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest2.main() # Real test using the antenna CDB
    else:
        simunittest.run(TestSetServoASConfigurationCmd)
