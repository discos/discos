from __future__ import with_statement

import os
import unittest

from PyMinorServoTest import simunittest
from Acspy.Clients.SimpleClient import PySimpleClient
from MinorServoErrors import MinorServoErrorsEx

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"

class TestSetElevationTracking(unittest.TestCase):

    telescope = os.getenv('STATION')

    @classmethod
    def setUpClass(cls):
        cls.client = PySimpleClient()
        cls.boss = cls.client.getComponent('MINORSERVO/Boss')
        
    @classmethod
    def tearDownClass(cls):
        cls.client.releaseComponent('MINORSERVO/Boss')

    def test_right_flag(self):
        """Set the elevation tracking properly"""
        self.boss.setElevationTracking('on')
        self.assertTrue(self.boss.isElevationTrackingEn())

    def test_wrong_flag(self):
        """Raise a MinorServoErrorsEx in case of wrong code"""
        with self.assertRaises(MinorServoErrorsEx):
            self.boss.setElevationTracking('foo')


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest.main() # Real test using the antenna CDB
    else:
        simunittest.run(TestSetElevationTracking)
