from __future__ import with_statement

import os

import unittest2

from PyMinorServoTest import simunittest
from Acspy.Clients.SimpleClient import PySimpleClient
from MinorServoErrors import MinorServoErrorsEx

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"

class TestGetCentralScanPosition(unittest2.TestCase):

    def setUp(self):
        client = PySimpleClient()
        self.boss = client.getComponent('MINORSERVO/Boss')

    def test_scan_not_active(self):
        """Raise a MinorServoErrorsEx if the scan is not active"""
        with self.assertRaises(MinorServoErrorsEx):
            self.boss.getCentralScanPosition()


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest2.main() # Real test using the antenna CDB
    else:
        simunittest.run(TestGetCentralScanPosition)

