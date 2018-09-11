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


class TestSetServoOffsetCmd(unittest.TestCase):
    """Test the setServoOffset command"""

    telescope = os.getenv('STATION')

    def setUp(self):
        self.client = PySimpleClient()
        self.boss = self.client.getComponent('MINORSERVO/Boss')

    def tearDown(self):
        self.boss.park()
        self.client.releaseComponent('MINORSERVO/Boss')

    def test_wrong_axis_code(self):
        success, answer = self.boss.command('setServoOffset=FOO_TX,0')
        self.assertFalse(success)


if __name__ == '__main__':
    unittest.main()
