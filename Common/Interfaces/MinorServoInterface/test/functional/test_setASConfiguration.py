from __future__ import with_statement

import os
import unittest2

from PyMinorServoTest import simunittest
from Acspy.Clients.SimpleClient import PySimpleClient
from MinorServoErrors import MinorServoErrorsEx

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"

class TestSetASConfiguration(unittest2.TestCase):

    def setUp(self):
        client = PySimpleClient()
        self.boss = client.getComponent('MINORSERVO/Boss')

    def test_right_flag(self):
        """Set the AS configuration properly"""
        self.boss.setASConfiguration('on')
        self.assertTrue(self.boss.isASConfiguration())

    def test_wrong_flag(self):
        """Raise a MinorServoErrorsEx in case of wrong code"""
        with self.assertRaises(MinorServoErrorsEx):
            self.boss.setASConfiguration('foo')


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest2.main() # Real test using the antenna CDB
    else:
        simunittest.run(TestSetASConfiguration)
