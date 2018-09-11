from __future__ import with_statement

import os
import unittest

from Acspy.Clients.SimpleClient import PySimpleClient
from MinorServoErrors import MinorServoErrorsEx

from acswrapper.system import acs
from acswrapper.containers import (
    Container, ContainerError, start_containers_and_wait,
    stop_containers_and_wait
)

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class TestSetASConfiguration(unittest.TestCase):

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
        
    @classmethod
    def tearDownClass(cls):
        cls.client.releaseComponent('MINORSERVO/Boss')
        stop_containers_and_wait(cls.containers)

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
        unittest.main() # Real test using the antenna CDB
    else:
        from testing import simulator
        simulator.run(TestSetASConfiguration, 'srt-mscu-sim')
