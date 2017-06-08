"""Test issue https://github.com/discos/discos/issues/143"""

import os
import time
import unittest
from subprocess import Popen, PIPE
from Acspy.Clients.SimpleClient import PySimpleClient
from testing.containers import Container


__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class TestContainerCrash(unittest.TestCase):

    telescope = os.getenv('STATION')

    @classmethod
    def setUpClass(cls):
        cls.containers = [
            Container('MinorServoContainer', 'cpp'),
        ]


    def setUp(self):
        self.client = PySimpleClient()
        for container in self.containers:
            container.start()
            container.wait_until_running()
            if not container.is_running():
                self.fail('cannot run %s' % container.name)

    def tearDown(self):
        self.client.disconnect()
        for container in self.containers:
            container.stop()
    
    def test_get_and_release_component(self):
        """The container must be alive after releasing the component"""
        srp = self.client.getComponent('MINORSERVO/SRP')
        self.client.releaseComponent('MINORSERVO/SRP')
        time.sleep(3)
        for container in self.containers:
            self.assertTrue(container.is_running())

        
if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest.main(verbosity=2, failfast=True) # Real test using the antenna CDB
    else:
        from PyMinorServoTest import simunittest
        simunittest.run(TestContainerCrash)
