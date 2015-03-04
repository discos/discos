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


class TestClearUserOffset(unittest2.TestCase):

    def setUp(self):
        client = PySimpleClient()
        self.boss = client.getComponent('MINORSERVO/Boss')

    def test_wrong_servo_name(self):
        """Raise a MinorServoErrorsEx in case of wrong servo name"""
        with self.assertRaises(MinorServoErrorsEx):
            self.boss.clearUserOffset('FOO')


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest2.main() # Real test using the antenna CDB
    else:
        simunittest.run(TestClearUserOffset)
