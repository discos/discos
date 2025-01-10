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

class TestGetCentralScanPosition(unittest.TestCase):

    telescope = os.getenv('STATION')

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
        
    @classmethod
    def tearDownClass(cls):
        stop_containers_and_wait(cls.containers)

    def setUp(self):
        self.client = PySimpleClient()
        self.boss = self.client.getComponent('MINORSERVO/Boss')

    def tearDown(self):
        self.client.releaseComponent('MINORSERVO/Boss')

    def test_scan_not_active(self):
        """Raise a MinorServoErrorsEx if the scan is not active"""
        with self.assertRaises(MinorServoErrorsEx):
            self.boss.getCentralScanPosition()


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest.main() # Real test using the antenna CDB
    else:
        from testing import simulator
        simulator.run(TestGetCentralScanPosition, 'srt-mscu-sim')

